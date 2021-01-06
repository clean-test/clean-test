# Ticket List


## General

 - [ ] Write README
 - [x] Add copyrights to files


## Build

 - [ ] Make build warnings / werror configurable
 - [ ] Make installable
 - [ ] Support building as part (i.e. subdir) of another cmake project.
       If so, honor static / dynamic building of the parent.
 - [ ] Make packageable (for github, vcpkg, etc.)
 - [ ] Make proper C++20 module


## Doc

 - [ ] Add generator
 - [ ] Provide short (tested) demos
 - [ ] Document toplevel API headers


## Execute

 - [x] Provide `main`-variant with `Configuration`
 - [ ] Support parsing `Configuration` with (printed) failure
 - [x] `main` general enough for `nutsh`?
 - [ ] (Restricted) support for non-exception environments
 - [ ] Multiline expression decomposition in output
 - [ ] Provide real parallel showcase (registration, expectation)
 - [ ] Timeout capabilities


## Framework

 - [x] Replace `std::function` in order to support move-only testcases\
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
 - [ ] Support fallback `Observer` (for users forgetting to pass it along)


## Expression

 - [ ] Support literals
 - [ ] Support unary not
 - [ ] Support more binary operators
 - [ ] Deal with configurable floating point accuracy
 - [ ] Comparison of containers
 - [ ] Think about case-insensitive string comparisons


## Utils

 - [x] Extend `ScopeGuard` exception specification; add tests.
