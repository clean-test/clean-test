.. Copyright m8mble 2020.
   SPDX-License-Identifier: BSL-1.0

**********
Clean Test
**********

.. image:: https://img.shields.io/badge/License-BSL--1.0-informational
   :target: LICENSE.txt

A modern C++-20 unit-testing framework for professionals.

==========
Motivation
==========

Many great unit-testing frameworks have been released so far.
Yet none combines the following qualities:

* **Macro free**:
  expression introspection without macros,
  leverages modern C++-20,
  easy to discover.

* **Fully parallel**:
  execute tests on all CPU cores,
  safely observe any parallel failures.

* **Production ready**:
  easy to integrate as CMake library without dependencies,
  JUnit reports for CI,
  full UTF-8 support,
  flexible runtime configuration.


====
Demo
====

.. code-block:: cpp

   #include <clean-test/clean-test.h>

   constexpr auto sum(auto... vs) { return (0 + ... + vs); }

   namespace ct = clean_test;
   using namespace ct::literals;

   auto const suite = ct::Suite{"sum", [] {
       "zero"_test = [] { ct::expect(sum() == 0_i); };
       "three"_test = [] { ct::expect(sum(1, 2) == 1 + 2_i); };
       "combined"_test = [] { ct::expect(sum(-1) < 0_i and sum(+1) > 0_i); };
   }};

   int main(int argc, char ** argv) {
       return ct::main(argc, argv);
   }

Even before consulting the docs, it should be fairly obvious to guess what happens here:
We have three simple tests for a standard ``sum`` function.
Since ``sum`` is correctly implemented, all test-cases will succeed.

But what happens if we would have written :code:`constexpr auto sum(auto... vs) { return (10 + ... + vs); }` instead?

.. code-block:: none

   Failure in ../test/Demo.cpp:12
   ( 10 == 0 )
   Failure in ../test/Demo.cpp:13
   ( 13 == ( 1 + 2 ) )
   Failure in ../test/Demo.cpp:14
   ( ( 9 < 0 ) and <unknown> )

Clean Test shows how different parts of an expectation are evaluated.
This works for constants, temporaries and even short circuiting operators alike.
The user-defined literals from :code:`clean_test::literals` support this introspection but are not mandatory.

By default Clean Test utilizes all available hardware threads to execute tests in parallel.
Moreover you can easily use Clean Test with your existing parallel setup (e.g. thread pool).

Many aspects of test execution can be configured dynamically.
It is possible to specify at runtime
which test cases should be selected, how they should be executed and what kind of report is desired as output.
Together with UTF-8 support this makes Clean Test a perfect fit for your CI-pipeline.
