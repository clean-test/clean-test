.. Copyright m8mble 2021.
   SPDX-License-Identifier: BSL-1.0

.. |Clean Test Logo| raw:: html

  <img src="_static/logo-light.svg" width="80%" class="only-light">
  <img src="_static/logo-dark.svg" width="80%" class="only-dark">

|Clean Test Logo|
=================
.. image:: https://github.com/clean-test/clean-test/actions/workflows/gcc.yml/badge.svg?branch=main
.. image:: https://github.com/clean-test/clean-test/actions/workflows/clang.yml/badge.svg?branch=main
.. image:: https://github.com/clean-test/clean-test/actions/workflows/msvc.yml/badge.svg?branch=main
.. image:: https://github.com/clean-test/clean-test/actions/workflows/doc.yml/badge.svg?branch=main
.. image:: https://img.shields.io/badge/c%2B%2B-20-informational
.. image:: https://img.shields.io/badge/license-BSL--1.0-informational

A modern C++-20 unit-testing framework.

.. toctree::
   :maxdepth: 2
   :caption: Content:
   :hidden:

   usage
   notions
   expressions
   api

Example
---------------------------------------

Writing tests is as simple as writing modern, standard C++-20:

.. literalinclude:: ../test/Demo.cpp
   :language: cpp
   :lines: 4-15


Key Features
---------------------------------------

Although many unit-testing frameworks have been released to date, none combines the following qualities:

Macro Free
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Macros come with many drawbacks: lack of scoping, difficulty to evolve, debug or understand (just to name a few).
This often adds up to *macro magic* which is hardly accessible or discoverable - even for experienced developers.

As a consequence macros are often discouraged in code bases.
In contrast to this most unit-testing frameworks heavily rely on macros, particularly for expressing assertions.

Clean Test does not need any macros, but leverages modern, standard C++ instead.
The resulting :doc:`expression introspection <expressions>` is highly superior
(:cpp:`constexpr`, supports short circuiting).
The few :doc:`notions of the framework <notions>` are very easy to discover for any C++ developer.

Parallel
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Modern hardware comes with multi-CPU architectures and many C++-programs make use of these.
However, the existing frameworks can only execute the corresponding unit-tests sequentially.

Clean Test on the other hand executes any number of tests simultaneously and can fully benefit from modern hardware.
This helps to execute tests more quickly and to identify issues with thread-safety already during testing.

The assertions of Clean Test are thread-safe i.e. simultaneous failures of one assertion can be detected.
This thread safety particularly applies to parallel tests (e.g. based on ``asio`` or some thread pool).

Production Ready
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Clean Test provides all the utilities expected from a unit-testing framework:
It has no dependencies and as a CMake library is very easy to build and integrate.

The flexible runtime configuration is particularly useful during development.
Many details can be specified dynamically e.g. test selection, execution context as well as output control.

Moreover Clean Test can also be used with any continuous integration tool.
With the support of JUnit XML-reports (the most common format) any CI-engine can display error details and statistics.
Clean Test guarantees valid UTF-8 output, thus allowing for assertions with arbitrary byte-sequences.
The necessary escaping is done automatically, if necessary.


Video
-------

Details about Clean Test were presented at Meeting C++ 2022 in Berlin:

.. image:: https://img.youtube.com/vi/JF83GGNvvM8/0.jpg
    :alt: Clean Test at Meeting C++ 2022
    :target: https://www.youtube.com/watch?v=JF83GGNvvM8
    :align: center
