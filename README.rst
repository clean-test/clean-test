.. Copyright m8mble 2020.
   SPDX-License-Identifier: BSL-1.0

**********
Clean Test
**********

.. image:: https://github.com/m8mble/clean-test/actions/workflows/gcc.yml/badge.svg
   :target: https://github.com/m8mble/clean-test/actions/workflows/gcc.yml
.. image:: https://github.com/m8mble/clean-test/actions/workflows/clang.yml/badge.svg
   :target: https://github.com/m8mble/clean-test/actions/workflows/clang.yml
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

Further details are elaborated in
the `official documentation <https://m8mble.github.io/clean-test-doc/latest/index.html>`_.


======
Status
======

Clean Test already provides all core features, but currently still lacks some convenience functionality.
There is a list of `envisioned features <doc/status.md>`_ that will be added in the coming weeks and months.

Clean Test consists of 100% standard C++-20 and works with any modern compiler toolchain.
As of now, it works both with gcc (version 10) and clang (version 11).
Once MSVC supports concepts it will also work on Windows.
