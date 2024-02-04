soft_double
==================

<p align="center">
    <a href="https://github.com/ckormanyos/soft_double/actions">
        <img src="https://github.com/ckormanyos/soft_double/actions/workflows/soft_double.yml/badge.svg" alt="Build Status"></a>
    <a href="https://github.com/ckormanyos/soft_double/issues?q=is%3Aissue+is%3Aopen+sort%3Aupdated-desc">
        <img src="https://custom-icon-badges.herokuapp.com/github/issues-raw/ckormanyos/soft_double?logo=github" alt="Issues"></a>
    <a href="https://scan.coverity.com/projects/ckormanyos-soft_double">
        <img src="https://scan.coverity.com/projects/26197/badge.svg" alt="Coverity Scan Build Status"></a>
    <a href="https://sonarcloud.io/summary/new_code?id=ckormanyos_soft_double">
        <img src="https://sonarcloud.io/api/project_badges/measure?project=ckormanyos_soft_double&metric=alert_status" alt="Quality Gate Status"></a>
    <a href="https://codecov.io/gh/ckormanyos/soft_double">
        <img src="https://codecov.io/gh/ckormanyos/soft_double/branch/main/graph/badge.svg?token=V1JNXSUQJQ"></a>
    <a href="https://github.com/ckormanyos/soft_double/blob/master/LICENSE_1_0.txt">
        <img src="https://img.shields.io/badge/license-BSL%201.0-blue.svg" alt="Boost Software License 1.0"></a>
    <a href="https://img.shields.io/github/commit-activity/y/ckormanyos/soft_double">
        <img src="https://img.shields.io/github/commit-activity/y/ckormanyos/soft_double" alt="GitHub commit activity"></a>
    <a href="https://github.com/ckormanyos/soft_double">
        <img src="https://img.shields.io/github/languages/code-size/ckormanyos/soft_double" alt="GitHub code size in bytes"></a>
    <a href="https://godbolt.org/z/oqh333Y33" alt="godbolt">
        <img src="https://img.shields.io/badge/try%20it%20on-godbolt-green" /></a>
</p>

soft_double is a C++ header-only library that implements
a 64-bit double-precision floating-point data type in software.
The data type implemented is called `::math::softfloat::soft_double`
and it can be used essentially like a regular
built-in 64-bit floating-point type (such as built-in `double`).
This data type type is also aliased to `::math::softfloat::float64_t`.

Some C/C++ environments do not support a built-in 64-bit floating-point type,
which might be commonly known as `double` or `long` `double`,
depending on the target system/compiler combination.
Consider, for instance, the `avr-gcc` tool chain popularly used on
8-bit embedded systems like Arduino or standalone _bare metal_ AVR.
Prior to version 10, this compiler does not support 64-bit `double`.

On such systems, soft_double can be used to provide a software-emulated,
portable implementation of 64-bit `double`.

soft_double implements common algebraic operations,
comparison operations, simple functions such as
`fabs`, `frexp`, `sqrt`, some power functions such as
`log`, `exp`, a few trigonometric functions including
`sin`, `cos`, and more. There is also full support/specialization
of `std::numeric_limits<soft_double>` for the `soft_double` type.

The soft_double implementation is written in header-only C++14
and is compatible for C++14, 17, 20, 23 and beyond.

## Quick Start

Using soft_double is straightforward. Simply `#include <math/softfloat/soft_double.h>` and
the `soft_double` type can be used almost like familiar built-in 64-bit `double`.
For instance,

```cpp
#include <math/softfloat/soft_double.h>

// Use a convenient alias for float64_t.
using ::math::softfloat::float64_t;

// Use the type like built-in `double`.
const float64_t one_third = float64_t(1U) / 3U;
```

An interesting detail in this code sample is the construction
of `one_third` from the composite initialization
provided by `float64_t(1U) / 3U`.
This is needed, for example, when the compiler
does not support 64-bit built-in `double` or `long` `double`
and these are limited to 32-bits (because
the 64-bit floating-point type is the actual type
that is being emulated). This situation arises,
as mentioned above, on certain popular versions of
the `avr-gcc` and other tool chains.

## Implementation goals

  - Clean header-only C++ design.
  - Seamless portability to any modern C++14, 17, 20, 23 compiler (and beyond).
  - Achieve efficiency suitable for _bare-metal_ embedded systems.
  - Particularly useful if 64-bit native `double` or a similar built-in type is unavailable.
  - Use refactored versions of trusted algorithms based on those found in [softfloat 3e](https://github.com/ucb-bar/berkeley-softfloat-3).

## Details

The following more detailed example provides an in-depth examination
into effectively using soft_double. This code computes and checks the value of

$$\sqrt{\pi} \approx 1.77245385090551602730 \ldots$$

This example, compiled with successful output result,
is shown in its entirety in the following
[short link](https://godbolt.org/z/j66jMqPT8) to [godbolt](https://godbolt.org).

```cpp
#include <cmath>
#include <iomanip>
#include <iostream>

#include <math/softfloat/soft_double.h>

int main()
{
  // Use a convenient alias for float64_t.
  using ::math::softfloat::float64_t;

  // Use a cached value for pi.
  const float64_t my_pi = float64_t::my_value_pi();

  // Compute soft_double sqrt(pi).
  const float64_t s = sqrt(my_pi);

  using std::sqrt;

  // Compare with native double sqrt(pi).
  const bool result_is_ok =
    (s.crepresentation() == float64_t(sqrt(3.1415926535897932384626433832795028841972)).crepresentation());

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}
```

When working with soft_double, performing library verification,
or prototyping project-specific software algorithms
(intended) to use the `soft_double` type, a built-in 64-bit `double`
can be helpful if available.
The examples, testing and verification of numerical correctness
in the soft_double project do, in fact, actually use 64-bit built-in `double`.

## Bare Metal Microcontroller

soft_double can readily be used _on the metal_ to emulate a 64-bit floating point type.
This can be provide the ability to do 64-bit floating-point calculations on
target system/compiler combinations lacking a built-in 64-bit floating-point type.

An example tested on various microcontrollers system
can be found in the file
[`example010_hypergeometric_2f1.cpp`](./examples/example010_hypergeometric_2f1.cpp).
This benchmark has also been tested on the 8-bit MICROCHIP ATmega328P controller
with `avr-gcc` versions 5 and 7 (both lacking built-in 64-bit `double`).

When working with even the most tiny microcontroller systems,
I/O streaming can optionally be disabled with the compiler switch:

```cpp
#define SOFT_DOUBLE_DISABLE_IOSTREAM
```

## Examples

Various interesting and algorithmically challenging
[examples](./examples) have been implemented.
It is hoped that the examples provide inspiration and guidance on
how to use soft_double.

  - ![`example001_roots_sqrt.cpp`](./examples/example001_roots_sqrt.cpp) computes a square root.
  - ![`example002b_pi_100k.cpp`](./examples/example002b_pi_100k.cpp) uses soft_double together with the [wide-decimal](https://github.com/ckormanyos/wide-decimal) project which stresses demanding use of real-world double-precision FFTs to compute $100,001$ decimal digits of $\pi$ using an iterative AGM method.
  - ![`example004_bessel_recur.cpp`](./examples/example004_bessel_recur.cpp) implements cylindrical Bessel functions of integral order via downward recursion with a Neumann sum.
  - ![`example005_polylog_series.cpp`](./examples/example005_polylog_series.cpp) performs a small-argument polylogarithm series calculation.
  - ![`example007_catalan_series.cpp`](./examples/example007_catalan_series.cpp) computes $\sim 15$ decimal digits of Catalan's constant using an accelerated series.
  - ![`example010_hypergeometric_2f1.cpp`](./examples/example010_hypergeometric_2f1.cpp) calculates $\sim 15$ decimal digits of a hypergeometric function value using a classic iterative rational approximation scheme.
  - ![`example011_trig_trapezoid_integral.cpp`](./examples/example011_trig_trapezoid_integral.cpp) uses trapezoid integration with an integral representation involving locally-written trigonometric sine and cosine functions to compute several cylindrical Bessel function values.
  - ![`example012_exercise_constexpr.cpp`](./examples/example012_exercise_constexpr.cpp) verifies that C++20 `constexpr`-ness works properly for both rudimentary assignment-operation as well as an elementary square root function.

## C++20 `constexpr` support

When using C++20, `soft_double` supports compile-time
`constexpr` construction and evaluation of results
of binary arithmetic, comparison operators
and various elementary functions.

The following code, for instance, shows a compile-time instantiation
of `soft_double` with subsequent `constexpr` evaluation
of a square root function and its comparison of its result
with the known control value.

See this example fully worked out at the following
[short link](https://godbolt.org/z/cqYWf4c31) to [godbolt](https://godbolt.org).
The generated assembly includes nothing other than the call to `main()`
and its subsequent `return` of the value zero
(i.e., `main()`'s successful return-value in this example).

```cpp
#include <cmath>

#include <math/softfloat/soft_double.h>

// Use a C++20 compiler for this example.

int main()
{
  // Use a convenient alias for float64_t.
  using ::math::softfloat::float64_t;

  // Use a cached value for pi.
  constexpr float64_t my_pi = float64_t::my_value_pi();

  // Compute soft_double sqrt(pi).
  constexpr float64_t s = sqrt(my_pi);

  constexpr auto result_is_ok =
    (s.crepresentation() == static_cast<typename float64_t::representation_type>(UINT64_C(0x3FFC5BF891B4EF6A)));

  // constexpr verification.
  // This is a compile-time comparison.
  static_assert(result_is_ok, "Error: example001_roots_sqrt not OK!");

  return (result_is_ok ? 0 : -1);
}
```

`constexpr`-ness of `soft_double` has been checked on GCC, clang
(with `-std=c++20` and beyond) and VC 14.2 and higher (with `/std:c++latest`),
also for various embedded compilers such as `avr-gcc` 11 and up,
`arm-non-eabi-gcc` 11 and up, and more.
In addition, less modern compiler versions have been sporadically
(not exhaustively) checked for `constexpr` usage of `soft_double`.
If you have an older compiler, you might have to check the compiler's
ability to obtain the entire benefit of `constexpr` with `soft_double`.

In [issue 110](https://github.com/ckormanyos/soft_double/issues/110),
the topic of `constexpr`-ness regarding construction from built-in `float`,
`double` and `long` `double` was briefly addressed. At the moment,
construction from built-in floating-point types adheres to C++20 `constexpr`-ness
or higher. Perhaps in the future, an alternate programing could bring this feature
to earlier language standards.

The macro sequence below can be used to test for the feature
of `constexpr`-ness regarding construction from built-in
floating-point types.

```cpp
#include <math/softfloat/soft_double.h>

#if ((defined SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS) && (SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS == 1))
#endif
```

It is not mandatory to actually use this feature-test if the
language standard being used is known to be sufficiently high
for compatibility. The following code, for instance, uses
`constexpr` construction from built-in `double`,
as shown also in this
[short link](https://godbolt.org/z/sovzoWTMc) to [godbolt](https://godbolt.org).

```cpp
#include <math/softfloat/soft_double.h>

auto main() -> int
{
  using ::math::softfloat::float64_t;

  constexpr auto gravitational_constant = float64_t { 6.674e-11 };
  constexpr auto near_pi_constant       = float64_t { 3.14 };
  constexpr auto one_quarter_constant   = float64_t { 0.25 };

  static_assert(gravitational_constant < 1, "Error: Initialization constexpr-double does not properly work");
  static_assert(gravitational_constant != near_pi_constant, "Error: Initialization constexpr-double does not properly work");
  static_assert(static_cast<int>(INT8_C(4)) * one_quarter_constant == 1, "Error: Initialization constexpr-double does not properly work");
  static_assert(static_cast<int>(INT8_C(12)) * one_quarter_constant < near_pi_constant, "Error: Initialization constexpr-double does not properly work");
  static_assert(static_cast<int>(INT8_C(13)) * one_quarter_constant > near_pi_constant, "Error: Initialization constexpr-double does not properly work");
}
```

## Building, testing and CI

### Build Status
[![Build Status](https://github.com/ckormanyos/soft_double/actions/workflows/soft_double.yml/badge.svg)](https://github.com/ckormanyos/soft_double/actions)

The recent status of building and executing the tests and examples
in Continuous Integration (CI) is always shown in the Build Status banner.

Simply using the [`soft_double.h` header](./math/softfloat/soft_double.h)
can be accomplished by identifying the header within
its directory, including the header path and header in the normal C++ way.

Building and running the tests and examples can be accomplished
using the Microsoft VisualStudio solution workspace provided
in `soft_double.sln` located in the project's root directory.
It is also possible, if desired, to build and execute
the tests and examples using various different OS/compiler
combinations.

Testing is a big issue and a growing test suite
is in continued progress providing for tested,
efficient functionality on the PC and workstation.
The GitHub code is delivered with an affiliated MSVC project.
It uses easy-to-understand subroutines called from
[`main()`](https://github.com/ckormanyos/soft_double/blob/fa628ee77c002c1073256056f7dcdb57d45e758c/test/test.cpp#L13)
that exercise various test cases.

Furthermore,
the [`example010_hypergeometric_2f1.cpp`](./examples/example010_hypergeometric_2f1.cpp)
benchmark is built-for the 8-bit MICROCHIP ATmega328P controller
and it is also built-for and executed-on the simulated
32-bit ARM(R) Cortex(R)-M4F in QEMU in CI.

CI runs on push and pull request using GitHub Actions.
Various compilers, operating systems, and various
C++ standards are included in CI.
