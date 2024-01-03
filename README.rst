.. Copyright m8mble 2020.
   SPDX-License-Identifier: BSL-1.0

.. |Clean Test Logo| raw:: html

  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="doc/logo/logo-dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="doc/logo/logo-light.svg">
    <img src="doc/logo/logo-light.svg" width="80%">
  </picture>

*****************
|Clean Test Logo|
*****************

.. image:: https://github.com/clean-test/clean-test/actions/workflows/gcc.yml/badge.svg?branch=main
   :target: https://github.com/clean-test/clean-test/actions/workflows/gcc.yml
.. image:: https://github.com/clean-test/clean-test/actions/workflows/clang.yml/badge.svg?branch=main
   :target: https://github.com/clean-test/clean-test/actions/workflows/clang.yml
.. image:: https://github.com/clean-test/clean-test/actions/workflows/msvc.yml/badge.svg?branch=main
   :target: https://github.com/clean-test/clean-test/actions/workflows/msvc.yml
.. image:: https://github.com/clean-test/clean-test/actions/workflows/doc.yml/badge.svg?branch=main
   :target: https://github.com/clean-test/clean-test/actions/workflows/doc.yml
.. image:: https://img.shields.io/badge/c%2B%2B-20-informational
   :target: https://en.wikipedia.org/wiki/C%2B%2B20
.. image:: https://img.shields.io/badge/license-BSL--1.0-informational
   :target: LICENSE.txt

A modern C++-20 unit-testing framework.


==========
Motivation
==========

Many unit-testing frameworks have been released so far.
Yet none combines the following qualities:

* **Macro free**:
  expression introspection without macros,
  leverages modern C++-20,
  easy to discover.

* **Parallel**:
  executes any number of tests simultaneously,
  utilizes all CPU cores,
  reliably reports failures.

* **Production ready**:
  easy to integrate as CMake library without dependencies,
  JUnit reports,
  UTF-8 support,
  flexible runtime configuration.

Further details are elaborated in the `official documentation <https://clean-test.github.io>`_.


=====
Demo
=====

.. role:: cpp(code)
   :language: cpp

Let's start with a basic example:
Consider a standard :cpp:`sum` function together with three tests to ensure its correctness.

.. code-block:: cpp

   #include <clean-test/clean-test.h>

   constexpr auto sum(auto... vs) { return (0 + ... + vs); }

   namespace ct = clean_test;
   using namespace ct::literals;

   auto const suite = ct::Suite{"sum", [] {
       "0"_test = [] { ct::expect(sum() == 0_i); };
       "3"_test = [] { ct::expect(sum(1, 2) == 1 + 2_i); };
       "A"_test = [] { ct::expect(sum(-1) < 0_i and sum(+1) > 0_i); };
   }};

Since ``sum`` is correctly implemented, all tests will succeed.
But what would happen if we made a mistake and wrote
:cpp:`constexpr auto sum(auto... vs) { return (10 + ... + vs); }` instead?

.. code-block:: none

   Failure in ../test/Demo.cpp:12
   ( 10 == 0 )
   Failure in ../test/Demo.cpp:13
   ( 13 == ( 1 + 2 ) )
   Failure in ../test/Demo.cpp:14
   ( ( 9 < 0 ) and <unknown> )

Clean Test shows how different parts of an :code:`ct::expect`-ation are evaluated.
This works for constants, temporaries and even short circuiting operators alike.
The user-defined literals from :cpp:`namespace clean_test::literals` can be used to support this introspection
but are not mandatory.

By default Clean Test utilizes all available hardware threads to execute tests in parallel.
Failure detection and reporting are thread-safe - even if your tests are parallel themselves.

All aspects of test execution can be configured dynamically.
It is possible to specify at runtime
which test cases should be selected, how they should be executed and what kind of reporting is desired.
Clean Test ensures valid UTF-8 reports and thus can safely be utilized in your CI-pipelines.


======
Video
======

Details about Clean Test were presented at Meeting C++ 2022 in Berlin:

.. image:: https://img.youtube.com/vi/JF83GGNvvM8/0.jpg
    :alt: Clean Test at Meeting C++ 2022
    :target: https://www.youtube.com/watch?v=JF83GGNvvM8
    :align: center

======
Status
======

Clean Test already provides all core features, but currently still lacks some convenience functionality.
There is a list of `envisioned features <doc/status.md>`_ that will be added in the coming weeks and months.

Clean Test consists of 100% standard C++-20 and works with any modern compiler toolchain.
It works with gcc (version 10 and above) and Clang (version 11 and above) on Linux as well as
with MSVC (version 19.29 and above) on Windows.
