soft_double
==================

<p align="center">
    <a href="https://github.com/ckormanyos/soft_double/actions">
        <img src="https://github.com/ckormanyos/soft_double/actions/workflows/soft_double.yml/badge.svg" alt="Build Status"></a>
    <a href="https://github.com/ckormanyos/soft_double/issues?q=is%3Aissue+is%3Aopen+sort%3Aupdated-desc">
        <img src="https://custom-icon-badges.herokuapp.com/github/issues-raw/ckormanyos/soft_double?logo=github" alt="Issues" /></a>
    <a href="https://sonarcloud.io/summary/new_code?id=ckormanyos_soft_float">
        <img src="https://sonarcloud.io/api/project_badges/measure?project=ckormanyos_soft_float&metric=alert_status" alt="Quality Gate Status"></a>
    <a href="https://github.com/ckormanyos/soft_double/blob/master/LICENSE_1_0.txt">
        <img src="https://img.shields.io/badge/license-BSL%201.0-blue.svg" alt="Boost Software License 1.0"></a>
    <a href="https://img.shields.io/github/commit-activity/y/ckormanyos/soft_double">
        <img src="https://img.shields.io/github/commit-activity/y/ckormanyos/soft_double" alt="GitHub commit activity" /></a>
    <a href="https://github.com/ckormanyos/soft_double">
        <img src="https://img.shields.io/github/languages/code-size/ckormanyos/soft_double" alt="GitHub code size in bytes" /></a>
</p>

soft_double is a C++ header-only library that implements
a 64-bit double-precision floating-point data type in software.
This data type is called `math::softfloat::soft_double`
and it can be used essentially like a regular
built-in 64-bit floating-point type (such as built-in `double`).

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

The soft_double implementation is written in header-only C++11
and is compatible for C++11, 14, 17, 20.

## Quick Start

Using soft_double is straightforward. Simply `#include <math/softfloat/soft_double.h>` and
the `soft_double` type can be used almost like familiar built-in 64-bit `double`.
For instance,

```cpp
#include <math/softfloat/soft_double.h>

// Use a convenient alias for float64_t.
using float64_t = math::softfloat::soft_double;

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
  - Seamless portability to any modern C++11, 14, 17, 20 compiler.
  - Achieve efficiency suitable for _bare-metal_ embedded systems.
  - Particularly useful if 64-bit native `double` or a similar built-in type is unavailable.
  - Use refactored versions of trusted algorithms based on those found in [softfloat 3e](https://github.com/ucb-bar/berkeley-softfloat-3).

## Details

The following more detailed example provides an in-depth examination
into effectively using soft_double. This code computes and checks the value of

$$\sqrt{\pi} \approx 1.77245385090551602730 \ldots$$

```cpp
#include <cmath>
#include <iomanip>
#include <iostream>

#include <math/softfloat/soft_double.h>

int main()
{
  // Use a convenient alias for float64_t.
  using float64_t = math::softfloat::soft_double;

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
[app_benchmark_soft_double_h2f1.cpp](https://github.com/ckormanyos/real-time-cpp/blob/master/ref_app/src/app/benchmark/app_benchmark_soft_double_h2f1.cpp)
in the [real-time-cpp](https://github.com/ckormanyos/real-time-cpp) repository.
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

## C++14, 17, 20 `constexpr` support

When using C++20, `soft_double` supports compile-time
`constexpr` construction and evaluation of results
of binary arithmetic, comparison operators
and various elementary functions.
The following code, for instance, shows a compile-time instantiation
of `soft_double` with subsequent `constexpr` evaluation
of a square root function and its comparison of its result
with the known control value.

```cpp
#include <math/softfloat/soft_double.h>

// Use a C++20 compiler for this example.

// Use a cached value for pi.
SOFT_DOUBLE_CONSTEXPR float64_t my_pi = float64_t::my_value_pi();

// Compute soft_double sqrt(pi).
SOFT_DOUBLE_CONSTEXPR float64_t s = sqrt(my_pi);

using std::sqrt;

// Compare with native double sqrt(pi).
// This is a run-time comparison.
const auto result_root_is_ok =
  (s.crepresentation() == float64_t(sqrt(3.1415926535897932384626433832795028841972)).crepresentation());

SOFT_DOUBLE_CONSTEXPR auto result_root_as_constexpr_is_ok =
  (s.crepresentation() == static_cast<typename float64_t::representation_type>(UINT64_C(0x3FFC5BF891B4EF6A)));

// constexpr verification.
// This is a compile-time comparison.
static_assert(result_root_as_constexpr_is_ok, "Error: example001_roots_sqrt not OK!");
```

`constexpr`-_ness_ of `soft_double` has been checked on GCC 10 and up, clang 10 and up
(with `-std=c++20`) and VC 14.2 (with `/std:c++latest`),
also for various embedded compilers such as `avr-gcc` 10 and up,
`arm-non-eabi-gcc` 10 and up, and more. In addition,
less modern compiler versions in addition to some other compilers
having standards such as C++14, 17, 2a have also been checked
for `constexpr` usage of `soft_double`. If you have an older
compiler, you might have to check the compiler's
ability to obtain the entire benefit of `constexpr` with `soft_double`.

If full `constexpr` compliance is not available or its
availability is unknown, the preprocessor symbols below can be useful.
These symbols are defined or set directly within the header(s)
of the soft_double library.

```cpp
SOFT_DOUBLE_CONSTEXPR
SOFT_DOUBLE_CONSTEXPR_IS_COMPILE_TIME_CONST
```

The preprocessor symbol `SOFT_DOUBLE_CONSTEXPR` acts as either
a synonym for `constexpr` or expands to nothing depending on
whether the availability of `constexpr` support has been automatically
detected or not.
The preprocessor symbol `SOFT_DOUBLE_CONSTEXPR_IS_COMPILE_TIME_CONST`
has the value of 0 or 1, where 1 indicates that `soft_double`
values qualified with `SOFT_DOUBLE_CONSTEXPR` are actually
compile-time constant (i.e., `constexpr`).

Detection of availability of `constexpr` support is implemented
[with preprocessor queries in soft_double.h](https://github.com/ckormanyos/soft_double/blob/2219a71a5e434c6c19e1fb33e7d5eedd7b6866a4/math/softfloat/soft_double.h#L71).
These complicated proprocessor queries are not complete (in the sense of
detecting all world-wide compiler/target systems). If you have
a specific compiler/target system needing `constexpr` detection,
please feel free to contact me directly so that this can be implemented.

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

CI runs on push and pull request using GitHub Actions.
Various compilers, operating systems, and C++ standards
ranging from C++11, 14, 17, 20 are included in CI.
