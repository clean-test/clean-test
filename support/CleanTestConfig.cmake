# Copyright (c) m8mble 2021.
# SPDX-License-Identifier: BSL-1.0

# Import central (generated) configuration details.
include(${CMAKE_CURRENT_LIST_FILE}/CleanTestConfigGenerated.cmake)

# Introduce automatic aliases
if (BUILD_SHARED_LIBS AND TARGET CleanTest::shared)
    add_library(CleanTest::automatic ALIAS CleanTest::shared)
    add_library(CleanTest::main-automatic ALIAS CleanTest::main-shared)
else ()
    add_library(CleanTest::automatic ALIAS CleanTest::static)
    add_library(CleanTest::main-automatic ALIAS CleanTest::main-static)
endif ()
