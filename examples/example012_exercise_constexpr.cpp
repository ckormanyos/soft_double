///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2023.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

// Use a C++20 compiler for this example.

auto math::softfloat::example012_exercise_constexpr() -> bool
{
  // Use a convenient alias for float64_t.
  using ::math::softfloat::float64_t;

  // Use a cached value for pi.
  constexpr float64_t my_pi = float64_t::my_value_pi();

  // Compute soft_double sqrt(pi).
  #if (defined(_MSC_VER) && (_MSC_VER < 1920))
  #pragma warning(push)
  #pragma warning(disable : 4307)
  #endif
  constexpr float64_t s = sqrt(my_pi);
  #if (defined(_MSC_VER) && (_MSC_VER < 1920))
  #pragma warning(pop)
  #endif

  using std::sqrt;

  // Compare with native double sqrt(pi).
  // This is a non-constexpr, run-time comparison.
  const auto result_root_non_constexpr_is_ok =
    (s.crepresentation() == float64_t(sqrt(3.1415926535897932384626433832795028841972)).crepresentation());

  constexpr auto result_root_as_constexpr_is_ok =
    (s.crepresentation() == static_cast<typename float64_t::representation_type>(UINT64_C(0x3FFC5BF891B4EF6A)));

  // constexpr verification.
  // This is a compile-time comparison.
  static_assert(result_root_as_constexpr_is_ok, "Error: example001_roots_sqrt not OK!");

  const auto result_is_ok = (result_root_non_constexpr_is_ok && result_root_as_constexpr_is_ok);

  return result_is_ok;
}


// Enable this if you would like to activate this main() as a standalone example.
#if 0 // NOLINT(readability-avoid-unconditional-preprocessor-if)

#include <iomanip>
#include <iostream>

auto main() -> int
{
  const bool result_is_ok = math::softfloat::example012_exercise_constexpr();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
