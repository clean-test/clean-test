// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "JUnitExport.h"

#include "XMLEncoder.h"

#include <utils/ScopeGuard.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <numeric>
#include <ostream>

#if __has_include(<unistd.h>)
# include <unistd.h>
#endif
#if __has_include(<winsock.h>)
# include <winsock.h>
#endif

namespace clean_test::execute {
namespace {

auto seconds(auto && x)
{
    return std::chrono::duration<float, std::chrono::seconds::period>{x}.count();
}

class Timestamp {
public:
    Timestamp() : m_time{std::time(nullptr)}
    {}

    friend std::ostream & operator<<(std::ostream & out, Timestamp const & t)
    {
        return out << std::put_time(std::localtime(&t.m_time), "%FT%T");
    }

private:
    std::time_t m_time;
};

class Hostname {
public:
    Hostname() : m_buffer{}
    {
        gethostname(m_buffer.data(), m_buffer.size());
    }

    friend std::ostream & operator<<(std::ostream & out, Hostname const & host)
    {
        return out << std::string_view{host.m_buffer.data()};
    }

private:
    std::array<char, 256> m_buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XMLStats {
public:
    explicit XMLStats(Outcome::Results const & results) :
        m_num_total{results.size()},
        m_num_failed{count<CaseStatus::fail>(results)},
        m_num_aborted{count<CaseStatus::abort>(results)}
    {}

    friend std::ostream & operator<<(std::ostream & out, XMLStats const & stats)
    {
        return out << "tests=\"" << stats.m_num_total << "\" "
                   << "failures=\"" << stats.m_num_failed << "\" disabled=\"0\" "
                   << "errors=\"" << stats.m_num_aborted << '"';
    }

private:
    template <CaseStatus::Value status>
    std::size_t count(Outcome::Results const & results)
    {
        return std::count_if(
            results.cbegin(), results.cend(), [](CaseResult const & r) { return r.m_status == status; });
    }

    std::size_t m_num_total;
    std::size_t m_num_failed;
    std::size_t m_num_aborted;
};

class XMLHead {
public:
    XMLStats m_stats;
    Outcome::Duration m_wall_time;

    friend std::ostream & operator<<(std::ostream & out, XMLHead const & head)
    {
        auto const & [stats, wall] = head;
        auto const now = Timestamp{};
        return out
            << R"(<?xml version="1.0" encoding="UTF-8"?>)" << '\n'

            << "<testsuites " << stats << " time=\"" << seconds(wall) << "\" timestamp=\"" << now << "\">\n"

            << " <testsuite name=\"/\" " << stats << " time=\"" << seconds(wall) << "\" timestamp=\"" << now << '\"'
            << " hostname=\"" << Hostname{} << "\" id=\"0\"" << ">\n";
    }
};

class XMLObservationMessage {
public:
    Observation const & m_observation;

    friend decltype(auto) operator<<(auto && destination, XMLObservationMessage const & msg)
    {
        auto const & [o] = msg;
        destination << o.m_where.file_name() << ':' << o.m_where.line() << '\n' << o.m_expression_details << '\n';
        if (not o.m_description.empty()) {
            destination << o.m_description << '\n';
        }
        return std::forward<decltype(destination)>(destination);
    }
};

class XMLObservation {
public:
    Observation const & m_observation;

    friend std::ostream & operator<<(std::ostream & out, XMLObservation const & xml)
    {
        auto const & [o] = xml;
        return out << "   <" << element(o.m_status)
                   << " message=\"" << XMLEncoder::On{} << XMLObservationMessage{o} << XMLEncoder::Off{} << "\""
                   << " type=\"expect\" />\n";
    }
private:
    static std::string_view element(ObservationStatus const status)
    {
        switch (status) {
            case ObservationStatus::pass:
                return "system-out";
            case ObservationStatus::fail:
            case ObservationStatus::fail_flaky:
                return "failure";
            case ObservationStatus::fail_asserted:
                return "error";
            default:
                std::terminate();
        }
    }
};

class XMLCase {
public:
    CaseResult const & result;

    friend std::ostream & operator<<(std::ostream & out, XMLCase const & c)
    {
        static auto const status_description = std::array<std::string_view, CaseStatus::num_values>{
            "passed", "failed", "aborted"};

        auto const & [r] = c;
        auto const & obs = r.m_observations;
        auto const has_children = not obs.empty() or (r.m_status == CaseStatus::skip);
        out << "  <testcase name=\"" << r.m_name_path << "\" status=\"" << status_description[r.m_status]
            << "\" time=\"" << seconds(r.m_wall_time) << "\"" << (has_children ? "" : " /") << ">\n";
        for (auto const & o : obs) {
            out << XMLObservation{o};
        }
        if (r.m_status == CaseStatus::skip) {
            out << "   <skipped />\n";
        }
        if (has_children) {
            out << "  </testcase>\n";
        }
        return out;
    }
};

class XMLCases {
public:
    Outcome::Results const & results;

    friend std::ostream & operator<<(std::ostream & out, XMLCases const & c)
    {
        for (auto const & r : c.results) {
            out << XMLCase{r};
        }
        return out;
    }
};

class XMLTail {
public:
    friend std::ostream & operator<<(std::ostream & out, XMLTail const &)
    {
        return out << " </testsuite>\n"
                   << "</testsuites>\n";
    }
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream & operator<<(std::ostream & out, JUnitExport data)
{
    auto const reset = utils::ScopeGuard{[&out, precision = out.precision()] {
        out.precision(precision);
        out.unsetf(std::ios_base::floatfield); // std::defaultfloat
    }};

    auto const & [wall_time, results] = data.m_outcome;
    return out
        << std::setprecision(3) << std::fixed
        << XMLHead{XMLStats{results}, wall_time} << XMLCases{results} << XMLTail{};
}

}
