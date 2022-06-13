// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "TreeDisplay.h"

#include <framework/Case.h>
#include <framework/Name.h>

#include <functional>
#include <limits>

namespace clean_test::execute {
namespace {

using Name = framework::Name;

constexpr auto list_continuation = std::string_view{"\u2502"};
constexpr auto list_item = std::string_view{"\u251C"};
constexpr auto list_end = std::string_view{"\u2514"};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Node {
public:
    using SubTree = std::map<std::string_view, Node>;
    std::vector<std::reference_wrapper<Name const>> names; //!< names with this composite name(-path)
    SubTree subtree; //!< children
};

class TreePosition {
public:
    using Position = Node::SubTree::const_iterator;

    TreePosition(Node::SubTree const & tree) :
        m_indent{list_continuation}, m_position{std::begin(tree)}, m_end{std::end(tree)}, m_num_remaining{tree.size()}
    {}

    bool is_last() const
    {
        return m_num_remaining == 1ul;
    }

    bool is_valid() const
    {
        return m_position != m_end;
    }

    void operator++()
    {
        ++m_position;
        --m_num_remaining;
    }

    decltype(auto) operator*() const
    {
        return *m_position;
    }

    decltype(auto) operator->() const
    {
        return std::addressof(**this);
    }

    std::string_view m_indent;

private:
    Position m_position;
    Position const m_end;
    std::size_t m_num_remaining; //!< Count remaining siblings down (rather than repeatedly checking @c m_position + 1.
};

using Stack = std::vector<TreePosition>;

/// Non-recursive, depth-first tree traversal.
template <std::invocable<Stack const &, TreePosition &> App>
void walk_tree(Node const & root, App && app)
{
    if (root.subtree.empty()) {
        return;
    }
    auto stack = std::vector{TreePosition{root.subtree}};
    while (not stack.empty()) {
        auto & cur = stack.back();

        app(stack, cur);

        stack.emplace_back(TreePosition{cur->second.subtree});
        while (not stack.back().is_valid()) {
            stack.pop_back();
            if (stack.empty()) {
                break;
            }
            ++stack.back();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

auto load_as_tree(TreeDisplay::Input const & input, NameFilter const & filter, std::size_t const max_depth)
{
    auto root = Node{};
    for (auto const & tc : input) {
        auto const & name = tc.name();
        if (not static_cast<bool>(filter(name))) {
            continue;
        }

        auto * node = std::addressof(root);
        auto path = name.path();
        auto depth = 0ul;
        for (auto pos = path.find(Name::separator); pos != std::string_view::npos and depth < max_depth;
             path.remove_prefix(pos + 1ul), pos = path.find(Name::separator), ++depth)
        {
            node = std::addressof(node->subtree[path.substr(0ul, pos)]);
        }
        if (depth < max_depth) {
            node->subtree[path].names.emplace_back(name);
        }
    }
    return root;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Indent {
public:
    Stack const & m_stack;

    friend std::ostream & operator<<(std::ostream & out, Indent const & indent)
    {
        out << "  ";
        for (auto i = 0ul; i < indent.m_stack.size() - 1u; ++i) {
            out << indent.m_stack[i].m_indent << "   ";
        }
        return out;
    }

private:
};

class Tags {
public:
    Name::Tags const & tags;

    friend std::ostream & operator<<(std::ostream & out, Tags const & t)
    {
        out << " {";
        auto sep = std::string_view{};
        for (auto const & tag : t.tags) {
            out << sep << std::string_view{tag};
            sep = ", ";
        }
        return out << '}';
    }
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TreeDisplay::TreeDisplay(Input const & cases, Setup setup) : m_cases{cases}, m_setup{std::move(setup)}
{}

std::ostream & operator<<(std::ostream & out, TreeDisplay const & data)
{
    auto const & [colors, filter, depth] = data.m_setup;
    auto const root = load_as_tree(data.m_cases, filter, depth == 0 ? std::numeric_limits<std::size_t>::max() : depth);

    walk_tree(root, [&](auto const & stack, auto & cur) {
        auto const & [dsc, node] = *cur;
        auto const & [names, subtree] = node;
        for (auto n = 0ul; n < names.size(); ++n) {
            auto const & name = names[n];
            auto const indent = (cur.is_last() and n + 1 == names.size() ? list_end : list_item);
            out << Indent{stack} << indent << ' ' << ColorTable::ColoredText{data.m_setup.m_colors, Color::good, dsc};
            auto const & tags = name.get().tags();
            if (not tags.empty()) {
                out << ' ' << Tags{tags};
            }
            out << '\n';
        }
        if (names.empty()) {
            auto const indent = (cur.is_last() ? list_end : list_item);
            out << Indent{stack} << indent << ' ' << dsc << '\n';
        }

        if (cur.is_last()) {
            cur.m_indent = " "; // if list got ended above, skip further list-continuation symbols
        }
    });
    return out;
}

}
