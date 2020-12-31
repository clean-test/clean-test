# Copyright (c) m8mble 2020.
# SPDX-License-Identifier: BSL-1.0

cmake_minimum_required(VERSION 3.17)
project(clean-test CXX)

include(CMakeParseArguments)

set(CMAKE_CXX_STANDARD 20)

# Configure build targets
option(CLNTST_TEST "Build tests for the framework." ON)
option(CLNTST_BUILD_STATIC "Whether to include building the statically linked library version." ON)
option(CLNTST_BUILD_SHARED "Whether to include building the shared, dynamically linked library version." ON)

if (CLNTEST_BUILD_STATIC AND APPLE)
    message(FATAL_ERROR "Statically linking not supported on Apple")
endif()
if (NOT CLNTEST_BUILD_STATIC AND NOT CLNTST_BUILD_SHARED)
    message(FATAL_ERROR "Both static and shared library building disabled. What do you want?")
endif()

# build configuration
add_compile_options(-Wall -Wextra -pedantic -Werror -Wswitch-enum)

function(add_files VAR prefix)
    set(content ${${VAR}})
    foreach (file ${ARGN})
        set(content ${content} ${prefix}/${file})
    endforeach()
    set(${VAR} ${content} PARENT_SCOPE)
endfunction()

add_files(CLNTST_PUBLIC_HEADERS include/clean-test
    clean-test.h
    execute.h
    expression.h
    framework.h
)
add_files(CLNTST_PUBLIC_HEADERS include/clean-test/expression
    Aborts.h
    And.h
    Base.h
    Clause.h
    Equal.h
    LazyDetector.h
    Lift.h
    Throws.h
)
add_files(CLNTST_PUBLIC_HEADERS include/clean-test/framework
    AbstractCaseRunner.h
    Case.h
    CaseRegistrar.h
    ConcreteCaseRunner.h
    Expect.h
    ExpectationObserver.h
    Name.h
    ObservationSetup.h
    ObserverFwd.h
    Registry.h
    SuiteRegistrar.h
    SuiteRegistrationSetup.h
    Tag.h
)
add_files(CLNTST_PUBLIC_HEADERS include/clean-test/execute
    Configuration.h
    Main.h
)
add_files(CLNTST_PUBLIC_HEADERS include/clean-test/utils
    FwdCapture.h
    Initializes.h
    ScopeGuard.h
    SourceLocation.h
)
add_files(CLNTST_SOURCES src/framework
    ExpectationObserver.cpp
    ObservationSetup.cpp
    Registry.cpp
    SuiteRegistrationSetup.cpp
)
add_files(CLNTST_SOURCES src/execute
    Abortion.h
    CaseEvaluator.cpp
    CaseEvaluator.h
    CaseReporter.cpp
    CaseReporter.h
    CaseResult.cpp
    CaseResult.h
    CaseStatus.h
    Configuration.cpp
    Main.cpp
    Observation.h
    ObservationStatus.h
    Observer.cpp
    Observer.h
)
add_files(CLNTST_SOURCES src/utils
    WithAdaptiveUnit.h
)

add_library(clntst-objlib OBJECT ${CLNTST_SOURCES})
target_include_directories(clntst-objlib PUBLIC include/)
target_include_directories(clntst-objlib PRIVATE include/clean-test)
target_include_directories(clntst-objlib PRIVATE src/)
if (CLNTST_BUILD_SHARED)
    set_property(TARGET clntst-objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

if (CLNTST_BUILD_STATIC)
    add_library(cleantest-static STATIC $<TARGET_OBJECTS:clntst-objlib> ${CLNTST_PUBLIC_HEADERS})
    target_include_directories(cleantest-static PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
    add_library(CleanTest::static ALIAS cleantest-static)
endif()
if (CLNTST_BUILD_SHARED)
    add_library(cleantest-shared SHARED $<TARGET_OBJECTS:clntst-objlib> ${CLNTST_PUBLIC_HEADERS})
    target_include_directories(cleantest-shared PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
    add_library(CleanTest::shared ALIAS cleantest-shared)
endif()
if (CLNTST_BUILD_STATIC AND NOT CLNTST_BUILD_SHARED)
    add_library(CleanTest::default ALIAS cleantest-static)
else ()
    add_library(CleanTest::default ALIAS cleantest-shared)
endif ()

if (CLNTST_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
