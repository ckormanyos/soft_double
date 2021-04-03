soft_double
[![Build Status](https://github.com/ckormanyos/soft_double/actions/workflows/soft_double.yml/badge.svg)](https://github.com/ckormanyos/soft_double/actions)
==================

soft_double provides a C++ software implementation of
a 64-bit double-precision floating-point data type.

Some compilers do not support common built-in 64-bit `double`.
The `avr-gcc` tool chain, for instance, commonly used
for embedded systems like Arduino
or standalone _bare metal_ AVR does not support 64-bit double
prior to versoin 10. soft_double can be used on such
systems to provide a software-emulated portable implementation
of 64-bit `double`.

This C++ template header-only library implements a drop-in double-precision
float type that can be used essentially like the regular built-in 64-bit
floating-point type `double`.

soft_double implements common algebraic operations,
`sqrt` and comparison operations,
and also includes full support for `std::numeric_limits`.

soft_double is written in header-only C++11, and compatible through C++14, 17, 20.

## Implementation goals

  - Clean header-only C++ design.
  - Seamless portability to any modern C++14, 17, 20 compiler (back-compatibility to C++11 is in progress).
  - Efficiency suitable for _bare-metal_ embedded systems, particularly if 64-bit.
  - Use trusted algorithms based on [softfloat 3e](  https://github.com/ucb-bar/berkeley-softfloat-3).

---

This work is in a preliminary state. Changes are planned including:
  - Better namespace names for the `soft_double` type.
  - Specialize further the template `std::numeric_limits<soft_double>`.
  - Consider header-only design.
  - Some `constexpr` sequences can't be resolved in C++11 and need C++14. Strive for C++11 through C++20 compatibility.
  - Add more tests and depth of CI.

---

## Examples

TBD

## Testing and CI

TBD
