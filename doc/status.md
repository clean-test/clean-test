# Ticket List


## Build

 - [ ] Make build warnings / werror configurable
 - [ ] Make installable
 - [ ] Support building as part (i.e. subdir) of another cmake project.
       If so, honor static / dynamic building of the parent.
 - [ ] Make packageable (for github, vcpkg, etc.)
 - [ ] Make proper C++20 module


## Doc

 - [ ] Add generator


## Framework

 - [x] Replace `std::function` in order to support move-only testcases\
 - [ ] Make output thread-safe by using `std::basic_osyncstream`
 - [x] Integrate Observer
 - [ ] Support parameterized tests (for loop; also with `operator|`?)
 - [ ] Decide whether delay registration is supported
       (i.e. registering while any case has already been started).
 - [ ] Make `Name` `constexpr` with `constexpr` `std::vector` / `std::string`.
       (`constexpr` consequences: `SuiteRegistrar`, UDLs, `Case`)
 - [ ] Normalize `Name` paths (e.g. strip `separator`).
 - [ ] Benchmark whether type-erasure via `std::function` is slower
       than doing this manually (via our own interface).
 - [x] Install (`thread_local`) observer upon test registration


## Expression

 - [ ] Support literals
 - [ ] Support unary not
 - [ ] Support more binary operators
 - [ ] Deal with configurable floating point accuracy
 - [ ] Comparison of containers


## Utils

 - [x] Extend `ScopeGuard` exception specification; add tests.
