///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

static_assert(sizeof(double) == 8U, // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
              "Error: This example requires 8 byte built-in double for verification");

namespace local
{
  template<typename FloatingPointType,
           typename UnsignedIntegralType>
  auto pow(FloatingPointType b, UnsignedIntegralType p) -> FloatingPointType
  {
    // Calculate (b ^ p).

    using floating_point_type = FloatingPointType;

    floating_point_type result;

    if     (p == 0) { result = floating_point_type(1U); }
    else if(p == 1) { result = b; }
    else if(p == 2) { result = b; result *= b; }
    else if(p == 3) { result = b; result *= b; result *= b; }
    else if(p == 4) { result = b; result *= b; result *= result; }
    else
    {
      result = floating_point_type(1U);

      floating_point_type y(b);

      for(auto p_local = static_cast<std::uint64_t>(p); p_local != static_cast<std::uint64_t>(UINT8_C(0)); p_local >>= 1U) // NOLINT(altera-id-dependent-backward-branch)
      {
        if(static_cast<std::uint8_t>(p_local & static_cast<std::uint8_t>(UINT8_C(1))) != static_cast<std::uint8_t>(UINT8_C(0)))
        {
          result *= y;
        }

        y *= y;
      }
    }

    return result;
  }

  template<typename FloatingPointType>
  auto polylog(const std::int32_t s, const FloatingPointType& x) -> FloatingPointType
  {
    using floating_point_type = FloatingPointType;

    const floating_point_type tol = (x * std::numeric_limits<floating_point_type>::epsilon()) / 10U;

    floating_point_type x_pow_k(x);
    floating_point_type sum    (x);

    for(std::uint_fast32_t k = UINT32_C(2); k < UINT32_C(100000); ++k)
    {
      x_pow_k *= x;

      using local::pow;
      using std::pow;

      const floating_point_type k_pow_s = pow(floating_point_type(k), s);
      const floating_point_type term    = x_pow_k / k_pow_s;

      if(term < tol)
      {
        break;
      }

      sum += term;
    }

    return sum;
  }
} // namespace local

auto math::softfloat::example005_polylog_series() -> bool
{
  using std::fabs;

  const float64_t poly = local::polylog(7U, float64_t(17U) / 71U);

  // N[PolyLog[7, 17/71], 41]
  const float64_t control(0.23989099751201076665599565769828454152031);

  // Check the closeness of the result.
  const float64_t closeness = fabs(1 - fabs(poly / control));

  const bool result_is_ok = (closeness < (std::numeric_limits<float64_t>::epsilon() * 10));

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0 // NOLINT(readability-avoid-unconditional-preprocessor-if)

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = math::softfloat::example005_polylog_series();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
