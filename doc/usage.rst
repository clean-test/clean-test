.. Copyright m8mble 2021.
   SPDX-License-Identifier: BSL-1.0

Usage
================

Clean Test is a CMake project and as such can be integrated easily.

Setup
------------------

There are a number of ways how to get Clean Test.

Installation
^^^^^^^^^^^^^

The quickest way to a working setup is to install a binary artifact.
This can either be obtained from `a release <https://github.com/m8mble/clean-test/releases>`_
or (soon) your package manager of choice.

With this no further preparation is needed.
Feel free to skip ahead to the :ref:`integration details<Integration>`.

Build
^^^^^

Alternatively you can also build Clean Test from sources.
Depending on your needs there are a couple of options how to do this.


Standalone
**********

Compiling Clean Test is no different from any other CMake project:

.. code:: bash

    mkdir build
    cd build
    cmake <PATH-TO-CLEAN-TEST>
    cmake --build .
    cmake --install .

Subdirectory
************

Some projects prefer to include forks of their dependencies.
Assuming the Clean Test repository has been cloned to ``ext/clean-test``,
this path can be included using

.. code:: cmake

    add_subdirectory(ext/clean-test)

in your ``CMakeLists.txt``.
That imports all targets of Clean Test.

FetchContent
************

The source of Clean Test can also be downloaded using CMake.
This can be done using the ``FetchContent`` module:

.. code:: cmake

    include(FetchContent)
    fetchcontent_declare(
      CleanTest
      GIT_REPOSITORY https://github.com/m8mble/clean-test.git
      GIT_TAG        main # or a fixed tag e.g. v1.2.0
    )
    fetchcontent_makeavailable(CleanTest)

With this snippet, CMake downloads and integrates a copy of Clean Test in your build directory.
Consider `the documentation <https://cmake.org/cmake/help/latest/module/FetchContent.html>`_ for further details.

Customization
***************

How Clean Test is built (in any of the above methods) can be configured by CMake options.
An important, generic setting is ``CMAKE_BUILD_TYPE`` (which can be used to enable ``Release`` builds).

Beyond the general configuration, Clean Test honors a few extra options:

- ``CLEANTEST_TEST`` whether the tests of Clean Test itself are built.
- ``CLEANTEST_BUILD_STATIC`` if the statically linked Clean Test library is built.
- ``CLEANTEST_BUILD_SHARED`` same for shared.

All three booleans are enabled by default.
How to link against any of the Clean Test libraries is described below.


Integration
-----------------
The Clean Test library comes in two flavors: statically and dynamically (shared) linked.

Some users consider writing the ``main`` of a test executable tedious.
To avoid this, you can also rather link against the ``main`` library of Clean Test.
It only contains an extra ``main`` that forwards to :cpp:`clean_test::main`.

CMake Projects
^^^^^^^^^^^^^^^

No matter how you obtained Clean Test, the integration is always the same:

.. code:: cmake

    add_executable(tests test-unit.cpp)
    target_link_libraries(tests PRIVATE CleanTest::main-automatic)
    add_test(NAME test COMMAND tests)

This adds the ``test`` executable to your CMake project.
Note that ``automatic`` selects between ``static`` and ``shared``
depending on your ``BUILD_SHARED_LIBS`` CMake setting.
If you rather prefer a fixed dependency, simply replace ``automatic`` by either ``shared`` or ``static``.

The ``main``-library already contains a top-level ``main`` entry point.
If you rather want to implement this yourself, feel free to link against Clean Test (without ``main``)
e.g. ``CleanTest::shared``.


Other Projects
^^^^^^^^^^^^^^^

The Clean Test core functionality comes both in a dynamically linked library (``libcleantest-shared.so``)
and a statically linked equivalent (``libcleantest-static.a``).
Both include all capabilities of Clean Test.
The optional, default ``main`` entry point is provided in ``libcleantest-main-static.a``.
