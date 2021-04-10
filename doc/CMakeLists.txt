# Copyright (c) m8mble 2020.
# SPDX-License-Identifier: BSL-1.0

find_program(DOXYGEN doxygen)
if (NOT DOXYGEN)
    message(STATUS "Target 'doc' disabled (requires doxygen)")
    return()
endif ()

find_program(POETRY poetry HINTS ~/.local/bin)
if (NOT POETRY)
    message(STATUS "Target 'doc' disabled (requires poetry)")
    return()
endif ()

add_custom_target(prepare-doc
    COMMAND ${POETRY} install
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(doc
    COMMAND ${POETRY} run generate --build-dir ${CMAKE_BINARY_DIR}
    COMMENT "Generate Clean Test documentation."
    VERBATIM
    SOURCES index.rst
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(upload-doc
    COMMAND ${POETRY} run upload --source ${CMAKE_BINARY_DIR}/html
    COMMENT "Update Clean Test online documentation."
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(doc prepare-doc)
add_dependencies(upload-doc doc)
