soft_double\
[![Build Status](https://github.com/ckormanyos/soft_double/actions/workflows/soft_double.yml/badge.svg)](https://github.com/ckormanyos/soft_double/actions)
==================

soft_double provides a C++ software implementation of
a 64-bit double-precision floating-point data type.

Some compilers do not support a built-in 64-bit floating-point type,
such as might be commonly known as `double` or `long` `double`.
Consider, for instance, the `avr-gcc` tool chain popularly used on
8-bit embedded systems like Arduino or standalone _bare metal_ AVR.
This compiler does not support 64-bit `double`
(prior to versoin 10).

soft_double can be used on such systems to provide a software-emulated,
portable implementation of 64-bit `double`.

soft_double is a C++ header-only library.
It implements a drop-in double-precision
floating-point type called `math::softfloat::soft_double`.
This type can be used essentially like the regular built-in 64-bit
floating-point type `double`.

soft_double implements common algebraic operations,
`sqrt` and comparison operations,
and also includes full support for `std::numeric_limits`.

soft_double is written in header-only C++11, and is compatible through C++11, 14, 17, 20.

## Implementation goals

  - Clean header-only C++ design.
  - Seamless portability to any modern C++11, 14, 17, 20 compiler.
  - Efficiency suitable for _bare-metal_ embedded systems, particularly if 64-bit native `double` or similar is unavailable.
  - Use refactored versoins of trusted algorithms based on those found in [softfloat 3e](https://github.com/ucb-bar/berkeley-softfloat-3).

## Quick Start

Using `soft_double` is simple. Simply `#include <math/soft_double/soft_double.h>` and
the `soft_double`type can be used almost like familiar built-in 64-bit `double`.
The following example, for instance, computes and checks the value of
<img src="https://render.githubusercontent.com/render/math?math=\sqrt{\pi}\,\approx\,1.77245385090551602730">.

```
#include <cmath>
#include <iomanip>
#include <iostream>

#include <math/soft_double/soft_double.h>

int main()
{
  // Use a convenient alias for float64_t.
  using float64_t = ::math::softfloat::soft_double;

  // Use a cached value for pi.
  const float64_t my_pi = float64_t::my_value_pi();

  // Compute soft_double sqrt(pi).
  const float64_t s = sqrt(my_pi);

  using std::sqrt;

  // Compare with native double sqrt(pi).
  const double control = sqrt(3.1415926535897932384626433832795028841972);

  union
  {
    double   d;
    uint64_t u;
  } uZ;

  uZ.d = control;

  const bool result_is_ok = (s.crepresentation() == uZ.u);

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}
```

## Testing and CI

Testing is a big issue and a growing test suite is in continued progress providing for tested,
efficient functionality on the PC and workstation. The GitHub code is delivered
with an affiliated MSVC project that uses easy-to-understand subroutines called
from `main()` that exercise various test cases. One of the tests uses
the [wide-decimal](https://github.com/ckormanyos/wide-decimal) project
in order to verify the functionality of `soft_double` with FFTs in a real-world application.

Continuous integration runs on push using GitHub Actions. Various compilers, operating systems,
and C++ standards ranging from C++11, 14, 17, 20 are included in CI.
