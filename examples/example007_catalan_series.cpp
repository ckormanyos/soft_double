///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>

#include <math/soft_double/soft_double.h>
#include <math/soft_double/soft_double_examples.h>

namespace
{
  using float64_t = math::softfloat::soft_double;

  template<typename FloatingPointType>
  FloatingPointType pi()
  {
    return FloatingPointType(3.1415926535897932384626433832795029L);
  }

  template<>
  float64_t pi()
  {
    return float64_t::my_value_pi();
  }

  template<typename FloatingPointType>
  FloatingPointType log_two_plus_sqrt_three()
  {
    using std::log;
    using std::sqrt;
    return log(2U + sqrt(FloatingPointType(3U)));
  }

  template<>
  float64_t log_two_plus_sqrt_three()
  {
    return float64_t(UINT64_C(0x3FF5124271980435), math::softfloat::detail::nothing());
  }
}

namespace local
{
  template<typename FloatingPointType>
  FloatingPointType catalan()
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
      const std::uint32_t tk = 2U * k;
      const std::uint64_t tk_plus_one_squared = (std::uint64_t) (tk + 1U) * (tk + 1U);

      k_fact  *= k;
      tk_fact *= (std::uint64_t) tk * (tk - 1U);

      floating_point_type term = (k_fact * k_fact) / (tk_fact * tk_plus_one_squared);

      sum += term;

      if(term < lim)
      {
        break;
      }
    }

    using ::pi;
    using ::log_two_plus_sqrt_three;

    const floating_point_type result =
      ((pi<floating_point_type>() * log_two_plus_sqrt_three<floating_point_type>()) + (sum * 3U)) / 8U;

    return result;
  }
}

bool math::softfloat::example007_catalan_series()
{
  const float64_t c = local::catalan<float64_t>();

  union
  {
    double   d;
    uint64_t u;
  } uZ;

  uZ.d = 0.9159655941772190150546035149323841107741;

  const float64_t control(uZ.u, ::math::softfloat::detail::nothing());

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
