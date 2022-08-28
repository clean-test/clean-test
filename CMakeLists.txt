# Copyright (c) m8mble 2020.
# SPDX-License-Identifier: BSL-1.0

cmake_minimum_required(VERSION 3.17)
project(clean-test CXX)
set(PROJECT_VERSION 0.2.0)

include(CMakeParseArguments)

set(CMAKE_CXX_STANDARD 20)

# Configure build targets
option(CLEANTEST_TEST "Build tests for the framework." ON)
option(CLEANTEST_BUILD_STATIC "Whether to include building the statically linked library version." ON)
option(CLEANTEST_BUILD_SHARED "Whether to include building the shared, dynamically linked library version." ON)
option(CLEANTEST_WERROR "Whether warnings should be treated as errors." OFF)

if (CLEANTEST_BUILD_STATIC AND APPLE)
    message(FATAL_ERROR "Statically linking not supported on Apple")
endif()
if (NOT CLEANTEST_BUILD_STATIC AND NOT CLEANTEST_BUILD_SHARED)
    message(FATAL_ERROR "Both static and shared library building disabled. What do you want?")
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CLEANTEST_WIN_SOCK32 "")
if (WIN32)
    set(CLEANTEST_WIN_SOCK32 wsock32 ws2_32)
endif ()

# Dependencies
find_package(Threads)

# build configuration
if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "CLANG"))
    add_compile_options(-Wall -Wextra -pedantic -Wswitch-enum -Wshadow)
    set(WERROR_FLAG -Werror)
endif ()
if (MSVC)
    add_compile_options(/W3)
    set(WERROR_FLAG /WX)
endif ()
if (CLEANTEST_WERROR)
    add_compile_options(${WERROR_FLAG})
endif()

function(add_files VAR prefix)
    set(content ${${VAR}})
    foreach (file ${ARGN})
        set(content ${content} ${prefix}/${file})
    endforeach()
    set(${VAR} ${content} PARENT_SCOPE)
endfunction()

add_files(CLEANTEST_PUBLIC_HEADERS include/clean-test
    clean-test.h
    execute.h
    expression.h
    framework.h
)
add_files(CLEANTEST_PUBLIC_HEADERS include/clean-test/expression
    Aborts.h
    Add.h
    Affirm.h
    And.h
    Base.h
    BitwiseAnd.h
    BitwiseNot.h
    BitwiseOr.h
    BitwiseXor.h
    Clause.h
    Close.h
    Comma.h
    Distance.h
    Divide.h
    Equal.h
    Greater.h
    GreaterEqual.h
    LazyDetector.h
    Less.h
    LessEqual.h
    Lift.h
    Literals.h
    Modulo.h
    Multiply.h
    Negate.h
    Not.h
    NotEqual.h
    Or.h
    ShortCircuitOperator.h
    StandardOperator.h
    Subtract.h
    Throws.h
    Tolerance.h
)
add_files(CLEANTEST_PUBLIC_HEADERS include/clean-test/framework
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
add_files(CLEANTEST_PUBLIC_HEADERS include/clean-test/execute
    BufferingMode.h
    ColoringMode.h
    Configuration.h
    Main.h
    NameFilterSetting.h
    OperationMode.h
)
add_files(CLEANTEST_PUBLIC_HEADERS include/clean-test/utils
    FwdCapture.h
    Initializes.h
    Math.h
    Pack.h
    Printable.h
    RemoveRvalueReference.h
    ScopeGuard.h
    SourceLocation.h
    UTF8Encoder.h
)
add_files(CLEANTEST_SOURCES src/framework
    ExpectationObserver.cpp
    FallbackObservationSetup.h
    ObservationSetup.cpp
    Registry.cpp
    SuiteRegistrationSetup.cpp
)
add_files(CLEANTEST_SOURCES src/execute
    Abortion.h
    Badges.cpp
    Badges.h
    CaseEvaluator.cpp
    CaseEvaluator.h
    CaseReporter.cpp
    CaseReporter.h
    CaseResult.cpp
    CaseResult.h
    CaseStatus.h
    ColorTable.cpp
    ColorTable.h
    ColoringSetup.cpp
    ColoringSetup.h
    Conductor.cpp
    Conductor.h
    Configuration.cpp
    HelpDisplay.cpp
    HelpDisplay.h
    JUnitExport.cpp
    JUnitExport.h
    Main.cpp
    NameFilter.cpp
    NameFilter.h
    Observation.h
    ObservationStatus.h
    Observer.cpp
    Observer.h
    Outcome.h
    TreeDisplay.cpp
    TreeDisplay.h
    XMLEncoder.cpp
    XMLEncoder.h
)
add_files(CLEANTEST_SOURCES src/utils
    Guarded.h
    OSyncStream.cpp
    OSyncStream.h
    RangesUtils.h
    UTF8Encoder.cpp
    UTF8Utils.cpp
    UTF8Utils.h
    WithAdaptiveUnit.h
)
add_files(CLEANTEST_MAIN_SOURCES src
    Main.cpp
)

add_library(clntst-objlib OBJECT ${CLEANTEST_SOURCES})
target_include_directories(clntst-objlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_include_directories(clntst-objlib PRIVATE include/clean-test)
target_include_directories(clntst-objlib PRIVATE src/)
if (CLEANTEST_BUILD_SHARED)
    set_property(TARGET clntst-objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

add_library(clntst-main-objlib OBJECT ${CLEANTEST_MAIN_SOURCES})
target_include_directories(clntst-main-objlib PRIVATE include/clean-test)
if (CLEANTEST_BUILD_SHARED)
    set_property(TARGET clntst-main-objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

if (CLEANTEST_BUILD_STATIC)
    add_library(cleantest-static STATIC $<TARGET_OBJECTS:clntst-objlib> ${CLEANTEST_PUBLIC_HEADERS})
    set_target_properties(cleantest-static PROPERTIES EXPORT_NAME static)
    target_include_directories(cleantest-static PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
    target_link_libraries(cleantest-static ${CMAKE_THREAD_LIBS_INIT} Threads::Threads ${CLEANTEST_WIN_SOCK32})
    add_library(CleanTest::static ALIAS cleantest-static)

    add_library(cleantest-main-static STATIC $<TARGET_OBJECTS:clntst-main-objlib>)
    set_target_properties(cleantest-main-static PROPERTIES EXPORT_NAME main-static)
    target_link_libraries(cleantest-main-static cleantest-static)
    add_library(CleanTest::main-static ALIAS cleantest-main-static)
endif()
if (CLEANTEST_BUILD_SHARED)
    add_library(cleantest-shared SHARED $<TARGET_OBJECTS:clntst-objlib> ${CLEANTEST_PUBLIC_HEADERS})
    set_target_properties(cleantest-shared PROPERTIES EXPORT_NAME shared)
    target_include_directories(cleantest-shared PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
    target_link_libraries(cleantest-shared ${CMAKE_THREAD_LIBS_INIT} Threads::Threads ${CLEANTEST_WIN_SOCK32})
    add_library(CleanTest::shared ALIAS cleantest-shared)

    add_library(cleantest-main-shared STATIC $<TARGET_OBJECTS:clntst-main-objlib>)
    set_target_properties(cleantest-main-shared PROPERTIES EXPORT_NAME main-shared)
    target_link_libraries(cleantest-main-shared cleantest-shared)
    add_library(CleanTest::main-shared ALIAS cleantest-main-shared)
endif()
if (CLEANTEST_BUILD_SHARED AND (BUILD_SHARED_LIBS OR NOT CLEANTEST_BUILD_STATIC))
    add_library(CleanTest::automatic ALIAS cleantest-static)
    add_library(CleanTest::main-automatic ALIAS cleantest-main-static)
else ()
    add_library(CleanTest::automatic ALIAS cleantest-shared)
    add_library(CleanTest::main-automatic ALIAS cleantest-main-shared)
endif ()

if (CLEANTEST_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
add_subdirectory(doc)

## Packaging ###########################################################################################################

# Skip if we are not the toplevel of the cmake build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CPACK_PACKAGE_NAME                clean-test)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A modern, C++-20 unit-testing framework.")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY   ${CPACK_PACKAGE_NAME})
    set(CPACK_PACKAGE_VERSION_MAJOR       ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR       ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH       ${PROJECT_VERSION_PATCH})
    set(CPACK_VERBATIM_VARIABLES          YES)
    set(CPACK_RESOURCE_FILE_LICENSE       ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt)
    set(CPACK_RESOURCE_FILE_README        ${CMAKE_CURRENT_LIST_DIR}/README.rst)
    set(CPACK_PACKAGE_CONTACT             m8mble@vivaldi.net)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER   m8mble)

    # Determine installation target directories
    include(GNUInstallDirs) # imports CMAKE_INSTALL_<dir> variables
    # overwrite doc dir (in case we are included as a child project)
    set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME})

    foreach(_TYPE shared static)
        if (TARGET CleanTest::${_TYPE})
            install(
                TARGETS cleantest-${_TYPE} cleantest-main-${_TYPE}
                EXPORT clean-test
                INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )
        endif()
    endforeach()
    install(
        EXPORT clean-test
        FILE CleanTestConfigGenerated.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CleanTest
        NAMESPACE CleanTest::
    )
    install(
        FILES support/CleanTestConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CleanTest
    )
    install(
        DIRECTORY include/clean-test
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    include(CPack)
endif()
