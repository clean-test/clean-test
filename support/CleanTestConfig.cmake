# Copyright (c) m8mble 2021.
# SPDX-License-Identifier: BSL-1.0

# Require dependencies
include(CMakeFindDependencyMacro)
find_dependency(Threads)

# Import central (generated) configuration details.
cmake_path(GET CMAKE_CURRENT_LIST_FILE PARENT_PATH CLEAN_TEST_GENERATED_INCLUDE_ROOT)
include(${CLEAN_TEST_GENERATED_INCLUDE_ROOT}/CleanTestConfigGenerated.cmake)

# Introduce automatic aliases
if (BUILD_SHARED_LIBS AND TARGET CleanTest::shared)
    add_library(CleanTest::automatic ALIAS CleanTest::shared)
    add_library(CleanTest::main-automatic ALIAS CleanTest::main-shared)
else ()
    add_library(CleanTest::automatic ALIAS CleanTest::static)
    add_library(CleanTest::main-automatic ALIAS CleanTest::main-static)
endif ()
