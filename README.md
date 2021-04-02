soft_double
[![Build Status](https://github.com/ckormanyos/soft_double/actions/workflows/soft_double.yml/badge.svg)](https://github.com/ckormanyos/soft_double/actions)
==================

soft_double provides a C++ software implementation of a double-precision floating-point data type

---

This work is in a preliminary state. Changes are planned including:
  - Better namespace names for the `soft_double` type.
  - Specialize further the template `std::numeric_limits<soft_double>`.
  - Consider header-only design.
  - Some `constexpr` sequences can't be resolved in C++11 and need C++14. Strive for C++11 through C++20 compatibility.
  - Add more tests and depth of CI.

---
