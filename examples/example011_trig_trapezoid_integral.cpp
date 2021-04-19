///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2021.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>
#include <limits>

#include <math/soft_double/soft_double.h>
#include <math/soft_double/soft_double_examples.h>

namespace
{
  using float64_t = math::softfloat::soft_double;
}

namespace local
{
  float64_t sin(float64_t x);
  float64_t cos(float64_t x);

  template<typename FloatType>
  FloatType pi() { return FloatType(); }

  template<>
  constexpr float pi() { return (float) 3.14159265358979323846264338327950288419716939937510582097L; }

  template<>
  constexpr double pi() { return (double) 3.14159265358979323846264338327950288419716939937510582097L; }

  template<>
  constexpr long double pi() { return (long double) 3.14159265358979323846264338327950288419716939937510582097L; }

  template<>
  constexpr float64_t pi() { return float64_t::my_value_pi(); }

  template<typename FloatType>
  FloatType pi_half() { return FloatType(); }

  template<>
  constexpr float pi_half() { return (float) 1.57079632679489661923132169163975144209858469968755291049L; }

  template<>
  constexpr double pi_half() { return (double) 1.57079632679489661923132169163975144209858469968755291049L; }

  template<>
  constexpr long double pi_half() { return (long double) 1.57079632679489661923132169163975144209858469968755291049L; }

  template<>
  constexpr float64_t pi_half() { return float64_t(UINT64_C(0x3FF921FB54442D18), math::softfloat::detail::nothing()); }

  float64_t sin_pade(float64_t x)
  {
    // PadeApproximant[Sin[x], {x, 0, {5,6}}]
    // FullSimplify[%]

    //   (42 x (  4363920 -  567120 x^2 +  12671 x^4))
    // /       (183284640 + 6728400 z^2 + 126210 z^4 + 1331 z^6)

    static const float64_t coef_sin_top_0(INT32_C(+4363920));
    static const float64_t coef_sin_top_1(INT32_C(-567120));
    static const float64_t coef_sin_top_2(INT32_C(+12671));

    static const float64_t coef_sin_bot_0(UINT32_C(+183284640));
    static const float64_t coef_sin_bot_1(UINT32_C(+6728400));
    static const float64_t coef_sin_bot_2(UINT32_C(+126210));
    static const float64_t coef_sin_bot_3(UINT32_C(+1331));


    const float64_t x2(x * x);

    const float64_t top = (((     + coef_sin_top_2)
                             * x2 + coef_sin_top_1)
                             * x2 + coef_sin_top_0);

    const float64_t bot = ((((     + coef_sin_bot_3)
                              * x2 + coef_sin_bot_2)
                              * x2 + coef_sin_bot_1)
                              * x2 + coef_sin_bot_0);

    return ((x * top) * 42U) / bot;
  }

  float64_t cos_pade(float64_t x)
  {
    // PadeApproximant[Cos[x] - 1, {x, 0, {6,6}}]
    // FullSimplify[%]

    // -(378 x^2
    //    (   51920 -    2800 x^2 +    39 x^4))
    //  / (39251520 + 1154160 x^2 + 16632 x^4 + 127 x^6)

    static const float64_t coef_cos_top_0(INT32_C(51920));
    static const float64_t coef_cos_top_1(INT32_C(-2800));
    static const float64_t coef_cos_top_2(INT32_C(39));

    static const float64_t coef_cos_bot_0(UINT32_C(39251520));
    static const float64_t coef_cos_bot_1(UINT32_C(1154160));
    static const float64_t coef_cos_bot_2(UINT32_C(16632));
    static const float64_t coef_cos_bot_3(UINT32_C(127));

    const float64_t x2(x * x);

    const float64_t top =  (((     + coef_cos_top_2)
                              * x2 + coef_cos_top_1)
                              * x2 + coef_cos_top_0);

    const float64_t bot = ((((     + coef_cos_bot_3)
                              * x2 + coef_cos_bot_2)
                              * x2 + coef_cos_bot_1)
                              * x2 + coef_cos_bot_0);

    return 1U - (((x2 * 378) * top) / bot);
  }

  float64_t sin(float64_t x)
  {
    float64_t s;

    if(x < 0)
    {
      s = -sin(-x);
    }
    else if(x > 0)
    {
      // Perform argument reduction and subsequent scaling of the result.

      // Given x = k * (pi/2) + r, compute n = (k % 4).

      // | n |  sin(x) |  cos(x) |  sin(x)/cos(x) |
      // |----------------------------------------|
      // | 0 |  sin(r) |  cos(r) |  sin(r)/cos(r) |
      // | 1 |  cos(r) | -sin(r) | -cos(r)/sin(r) |
      // | 2 | -sin(r) | -cos(r) |  sin(r)/cos(r) |
      // | 3 | -cos(r) |  sin(r) | -cos(r)/sin(r) |

      const std::uint32_t      k = (std::uint32_t) (x / pi_half<float64_t>());
      const std::uint_fast32_t n = (std::uint_fast32_t) (k % 4U);

      float64_t r = x - (pi_half<float64_t>() * k);

      const bool is_neg =  (n > 1U);
      const bool is_cos = ((n == 1U) || (n == 3U));

      std::uint_fast32_t n_angle_identity = 0U;

      static const float64_t two_tenths = float64_t(2U) / 10U;

      // Reduce the argument with factors of three until it is less than 2/10.
      while(r > two_tenths)
      {
        r /= 3U;

        ++n_angle_identity;
      }

      s = (is_cos ? cos_pade(r) : sin_pade(r));

      // Rescale the result with the triple angle identity for sine.
      for(std::uint_fast32_t t = 0U; t < n_angle_identity; ++t)
      {
        s = (s * 3U) - (((s * s) * s) * 4U);
      }

      s = fabs(s);

      if(is_neg)
      {
        s = -s;
      }
    }
    else
    {
      s = 0;
    }

    return s;
  }

  float64_t cos(float64_t x)
  {
    float64_t c;

    if(x < 0)
    {
      c = cos(-x);
    }
    else if(x > 0)
    {
      // Perform argument reduction and subsequent scaling of the result.

      // Given x = k * (pi/2) + r, compute n = (k % 4).

      // | n |  sin(x) |  cos(x) |  sin(x)/cos(x) |
      // |----------------------------------------|
      // | 0 |  sin(r) |  cos(r) |  sin(r)/cos(r) |
      // | 1 |  cos(r) | -sin(r) | -cos(r)/sin(r) |
      // | 2 | -sin(r) | -cos(r) |  sin(r)/cos(r) |
      // | 3 | -cos(r) |  sin(r) | -cos(r)/sin(r) |

      const std::uint32_t      k = (std::uint32_t) (x / pi_half<float64_t>());
      const std::uint_fast32_t n = (std::uint_fast32_t) (k % 4U);

      float64_t r = x - (pi_half<float64_t>() * k);

      const bool is_neg = ((n == 1U) || (n == 2U));
      const bool is_sin = ((n == 1U) || (n == 3U));

      std::uint_fast32_t n_angle_identity = 0U;

      static const float64_t two_tenths = float64_t(2U) / 10U;

      // Reduce the argument with factors of three until it is less than 2/10.
      while(r > two_tenths)
      {
        r /= 3U;

        ++n_angle_identity;
      }

      c = (is_sin ? sin_pade(r) : cos_pade(r));

      // Rescale the result with the triple angle identity for cosine.
      for(std::uint_fast32_t t = 0U; t < n_angle_identity; ++t)
      {
        c = (((c * c) * c) * 4U) - (c * 3U);
      }

      c = fabs(c);

      if(is_neg)
      {
        c = -c;
      }
    }
    else
    {
      c = float64_t(1U);
    }

    return c;
  }

  template<typename real_value_type,
           typename real_function_type>
  real_value_type integral(const real_value_type& a,
                           const real_value_type& b,
                           const real_value_type& tol,
                           real_function_type real_function)
  {
    std::uint_fast32_t n2(1);

    real_value_type step = ((b - a) / 2U);

    real_value_type result = (real_function(a) + real_function(b)) * step;

    const std::uint_fast8_t k_max = UINT8_C(32);

    for(std::uint_fast8_t k = UINT8_C(0); k < k_max; ++k)
    {
      real_value_type sum(0);

      for(std::uint_fast32_t j(0U); j < n2; ++j)
      {
        const std::uint_fast32_t two_j_plus_one = (j * UINT32_C(2)) + UINT32_C(1);

        sum += real_function(a + (step * two_j_plus_one));
      }

      const real_value_type tmp = result;

      result = (result / 2U) + (step * sum);

      using std::fabs;

      const real_value_type ratio = fabs(tmp / result);

      const real_value_type delta = fabs(ratio - 1U);

      if((k > UINT8_C(1)) && (delta < tol))
      {
        break;
      }

      n2 *= 2U;

      step /= 2U;
    }

    return result;
  }

  template<typename float_type>
  float_type cyl_bessel_j(const std::uint_fast8_t n,
                          const float_type& x)
  {
    const float_type epsilon = std::numeric_limits<float_type>::epsilon();

    using std::cos;
    using std::sin;
    using std::sqrt;

    using local::cos;
    using local::sin;

    const float_type tol = sqrt(epsilon);

    const float_type integration_result = integral(float_type(0),
                                                   pi<float_type>(),
                                                   tol,
                                                   [&x, &n](const float_type& t) -> float_type
                                                   {
                                                     return cos(x * sin(t) - (t * n));
                                                   });

    const float_type jn = integration_result / pi<float_type>();

    return jn;
  }
}

bool math::softfloat::example011_trig_trapezoid_integral()
{
  const float64_t j2 = local::cyl_bessel_j(2U, float64_t(123U) / 100U);
  const float64_t j3 = local::cyl_bessel_j(3U, float64_t(456U) / 100U);
  const float64_t j4 = local::cyl_bessel_j(4U, float64_t(789U) / 100U);

  // N[BesselJ[2, 123/100], 60]
  const float64_t control2 = float64_t(UINT64_C(0x3FC54B978B35C3D7), math::softfloat::detail::nothing());

  // N[BesselJ[3, 456/100], 60]
  const float64_t control3 = float64_t(UINT64_C(0x3FDAE7A3EDE1F738), math::softfloat::detail::nothing());

  // N[BesselJ[4, 789/100], 60]
  const float64_t control4 = float64_t(UINT64_C(0xBFB419069B8DC738), math::softfloat::detail::nothing());

  using std::fabs;

  const float64_t closeness2 = fabs(1 - fabs(j2 / control2));
  const float64_t closeness3 = fabs(1 - fabs(j3 / control3));
  const float64_t closeness4 = fabs(1 - fabs(j4 / control4));

  const float64_t tol = std::numeric_limits<float64_t>::epsilon() * 200U;

  const bool result_is_ok = (   (closeness2 < tol)
                             && (closeness3 < tol)
                             && (closeness4 < tol));

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = math::softfloat::example011_trig_trapezoid_integral();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
