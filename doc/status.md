[//]: <> (Copyright m8mble 2020.)
[//]: <> (SPDX-License-Identifier: BSL-1.0.)


# Ticket List


## General

 - [x] Write README
 - [x] Add copyrights to files


## Build

 - [x] Make build warnings / werror configurable
 - [x] Make installable
 - [x] Support building as part (i.e. subdir) of another cmake project.
       If so, honor static / dynamic building of the parent.
 - [ ] Make packageable (for github, vcpkg, etc.)
 - [ ] Make proper C++20 module
 - [x] Add static lib with already prepared main (mention in toplevel readme)


## Doc

 - [x] Add generator
 - [ ] Provide short (tested) demos
 - [ ] Document toplevel API headers


## Execute

 - [x] Provide `main`-variant with `Configuration`
 - [x] Support parsing `Configuration` with (printed) failure
 - [x] `main` general enough for `nutsh`?
 - [x] Validate utf-8
 - [ ] Provide real parallel showcase (registration, expectation)
 - [ ] (Restricted) support for non-exception environments
 - [ ] Multiline expression decomposition in output
 - [ ] Benchmark startup / execution speed


## Framework

 - [x] Replace `std::function` in order to support move-only testcases
 - [x] Make output thread-safe by using `std::basic_osyncstream`
 - [x] Integrate Observer
 - [ ] Support parameterized tests (for loop; also with `operator|`?)
 - [x] Decide whether delayed registration is supported
       (i.e. registering while any case has already been started).
 - [ ] Make `Name` `constexpr` with `constexpr` `std::vector` / `std::string`.
       (`constexpr` consequences: `SuiteRegistrar`, UDLs, `Case`)
 - [ ] Make `Tag`s owning (`std::string_view` -> `std::string`),
       s.t. users may store them without surprises.
 - [ ] Normalize `Name` paths (e.g. strip `separator`).
 - [x] Install (`thread_local`) observer upon test registration
 - [x] Move Observer into execute
 - [ ] Generate badges with `std::format`.
 - [x] Support fallback `Observer` (for users forgetting to pass it along)
 - [ ] Add scoped, `Observer`-based facility for additional debug output
 - [ ] Provide migration helpers for the big 3
 - [x] The unknown catch-all "test" shouldn't be counted in "Ran n test-cases" and not reported as failed (without errors)
 - [x] Single-threaded mode could allow a global observer (so that this works without errors)


## Expression

 - [x] Support literals
 - [x] Support unary not
 - [x] Support all binary operators
 - [ ] Deal with configurable floating point accuracy
 - [ ] Comparison of containers / ranges
 - [ ] Improve lazy detection: support lifting generators, improve descriptions (e.g. "throws").
 - [ ] Think about case-insensitive string comparisons
 - [ ] Diff for longer strings (colored?)


## Utils

 - [x] Extend `ScopeGuard` exception specification; add tests.
