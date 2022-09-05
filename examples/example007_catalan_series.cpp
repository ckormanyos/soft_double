///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

static_assert(sizeof(double) == 8U,
              "Error: This example requires 8 byte built-in double for verification");

namespace
{
  using float64_t = math::softfloat::soft_double;

  template<typename FloatingPointType>
  auto pi() -> FloatingPointType
  {
    return FloatingPointType(3.1415926535897932384626433832795029L);
  }

  template<>
  auto pi() -> float64_t
  {
    return float64_t::my_value_pi();
  }

  template<typename FloatingPointType>
  auto log_two_plus_sqrt_three() -> FloatingPointType
  {
    using std::log;
    using std::sqrt;
    return log(2U + sqrt(FloatingPointType(3U)));
  }

  template<>
  auto log_two_plus_sqrt_three() -> float64_t
  {
    return float64_t(UINT64_C(0x3FF5124271980435), math::softfloat::detail::nothing());
  }
}

namespace local
{
  template<typename FloatingPointType>
  auto catalan() -> FloatingPointType
  {
    using floating_point_type = FloatingPointType;

    // Adapted from Boost.Math.Constants (see file calculate_constants.hpp).
    // See also http://www.mpfr.org/algorithms.pdf

    floating_point_type k_fact (1U);
    floating_point_type tk_fact(2U);
    floating_point_type sum    (floating_point_type(19U) / 18U);

    const floating_point_type lim = std::numeric_limits<floating_point_type>::epsilon();

    for(std::uint_fast32_t k = UINT32_C(2); k < UINT32_C(10000000); ++k)
    {
      const auto tk                  = static_cast<std::uint32_t>(2U * k);
      const auto tk_plus_one         = static_cast<std::uint32_t>(tk + 1U);
      const auto tk_plus_one_squared = static_cast<std::uint64_t>(static_cast<std::uint64_t>(tk_plus_one) * tk_plus_one);

      k_fact  *= k;
      tk_fact *= (std::uint64_t) tk * (tk - 1U);

      floating_point_type term = (k_fact * k_fact) / (tk_fact * tk_plus_one_squared);

      sum += term;

      if(term < lim)
      {
        break;
      }
    }

    const floating_point_type result =
      ((pi<floating_point_type>() * log_two_plus_sqrt_three<floating_point_type>()) + (sum * 3U)) / 8U;

    return result;
  }
}

auto math::softfloat::example007_catalan_series() -> bool
{
  const float64_t c = local::catalan<float64_t>();

  // N[Catalan, 41]
  const float64_t control(0.9159655941772190150546035149323841107741);

  using std::fabs;

  const float64_t closeness = fabs(1 - (c / control));

  const bool result_is_ok = (closeness < (std::numeric_limits<float64_t>::epsilon() * 10));

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = math::softfloat::example007_catalan_series();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
