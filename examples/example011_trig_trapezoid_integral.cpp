///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2021 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>
#include <limits>

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

static_assert(sizeof(double) == 8U, // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
              "Error: This example requires 8 byte built-in double for verification");

namespace local { namespace detail {

using float64_t = math::softfloat::soft_double;

} // namespace detail

using detail::float64_t;

constexpr auto pi_value_as_long_double =
  static_cast<long double>
  (
    3.14159265358979323846264338327950288419716939937510582097L // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  );

template<typename FloatType>
auto pi() -> FloatType { return FloatType(); }

template<>
constexpr auto pi() -> float { return static_cast<float>(pi_value_as_long_double); }

template<>
constexpr auto pi() -> double { return static_cast<double>(pi_value_as_long_double); }

template<>
constexpr auto pi() -> long double { return static_cast<long double>(pi_value_as_long_double); }

template<>
constexpr auto pi() -> float64_t { return float64_t::my_value_pi(); }

template<typename real_value_type,
          typename real_function_type>
auto integral(const real_value_type&   a,
              const real_value_type&   b, // NOLINT(bugprone-easily-swappable-parameters)
              const real_value_type&   tol,
                    real_function_type real_function) -> real_value_type
{
  std::uint_fast32_t n2(1);

  real_value_type step = ((b - a) / 2U);

  real_value_type result = (real_function(a) + real_function(b)) * step;

  const std::uint_fast8_t k_max = UINT8_C(32);

  for(auto k = static_cast<std::uint_fast8_t>(UINT8_C(0)); k < k_max; ++k)
  {
    real_value_type sum(0);

    for(auto j = static_cast<std::uint_fast32_t>(UINT8_C(0)); j < n2; ++j)
    {
      const auto two_j_plus_one =
        static_cast<std::uint_fast32_t>
        (
          static_cast<std::uint_fast32_t>(j * UINT32_C(2)) + UINT32_C(1)
        );

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
auto cyl_bessel_j(const std::uint_fast8_t n, const float_type& x) -> float_type
{
  using std::sqrt;

  constexpr float_type eps = std::numeric_limits<float_type>::epsilon();

  const float_type tol = sqrt(eps);

  const float_type integration_result =
    integral
    (
      float_type(0),
      pi<float_type>(),
      tol,
      [&x, &n](const float_type& t) -> float_type
      {
        using std::cos;
        using std::sin;

        return cos(x * sin(t) - (t * n));
      }
    );

  const float_type jn = integration_result / pi<float_type>();

  return jn; // NOLINT(performance-no-automatic-move)
}

} // namespace local

auto math::softfloat::example011_trig_trapezoid_integral() -> bool
{
  const float64_t j2 = local::cyl_bessel_j(2U, float64_t(123U) / 100U);
  const float64_t j3 = local::cyl_bessel_j(3U, float64_t(456U) / 100U);
  const float64_t j4 = local::cyl_bessel_j(4U, float64_t(789U) / 100U);

  // N[BesselJ[2, 123/100], 41]
  const float64_t control2(0.16636938378681407351267852431513159437103);

  // N[BesselJ[3, 456/100], 41]
  const float64_t control3(0.42038820486765216162613462343078475742748);

  // N[BesselJ[4, 789/100], 41]
  const float64_t control4(-0.078506863572127438410485520328806569617327);

  using std::fabs;

  const float64_t closeness2 = fabs(1 - fabs(j2 / control2));
  const float64_t closeness3 = fabs(1 - fabs(j3 / control3));
  const float64_t closeness4 = fabs(1 - fabs(j4 / control4));

  const float64_t tol = std::numeric_limits<float64_t>::epsilon() * 40U;

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
