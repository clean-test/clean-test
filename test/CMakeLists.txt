# Copyright (c) m8mble 2020.
# SPDX-License-Identifier: BSL-1.0

# Shorthand for introducing a new test executable.
# Usage: add_clntst_test(name extra-srcs...)
function(add_clntst_test name)
    set(options EXTERNAL)
    set(oneValueArgs LINK)
    set(multiValueArgs)
    cmake_parse_arguments(PARSED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (NOT DEFINED PARSED_LINK)
        set(PARSED_LINK default)
    endif()

    string(TOLOWER ${name} EXE)
    set(sources "${name}.cpp")
    foreach (tu ${PARSED_UNPARSED_ARGUMENTS})
        list(APPEND sources "${tu}.cpp")
    endforeach ()

    add_executable(${EXE} ${sources})
    target_link_libraries(${EXE} CleanTest::${PARSED_LINK})
    if (NOT PARSED_EXTERNAL)
        # Add internal include paths (to src/) only when not configured with EXTERNAL.
        target_include_directories(${EXE} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
        target_include_directories(${EXE} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/clean-test>)
    endif()
    add_test(NAME ${EXE} COMMAND ${EXE})
endfunction()

add_clntst_test(Shared LINK shared EXTERNAL)
add_clntst_test(Static LINK static EXTERNAL)
add_clntst_test(Expression EXTERNAL)
add_clntst_test(Registration RegistrationAddendum EXTERNAL)
add_clntst_test(Expect)
add_clntst_test(ScopeGuard)
add_clntst_test(Complicated)
add_clntst_test(Guarded)
add_clntst_test(OSyncStream)
add_clntst_test(NameFilter)
