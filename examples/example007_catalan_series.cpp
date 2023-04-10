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

namespace local { namespace detail {

using float64_t = math::softfloat::soft_double;

template<typename FloatingPointType>
constexpr auto pi() -> FloatingPointType
{
  return FloatingPointType(3.1415926535897932384626433832795029L); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

template<>
constexpr auto pi() -> float64_t
{
  return float64_t::my_value_pi();
}

template<typename FloatingPointType>
constexpr auto log_two_plus_sqrt_three() -> FloatingPointType
{
  using std::log;
  using std::sqrt;
  return log(2U + sqrt(FloatingPointType(3U)));
}

template<>
constexpr auto log_two_plus_sqrt_three() -> float64_t
{
  return
  {
    static_cast<std::uint64_t>(UINT64_C(0x3FF5124271980435)), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    math::softfloat::detail::nothing()
  };
}

} // namespace detail

template<typename FloatingPointType>
constexpr auto catalan() -> FloatingPointType
{
  using floating_point_type = FloatingPointType;

  // Adapted from Boost.Math.Constants (see file calculate_constants.hpp).
  // See also http://www.mpfr.org/algorithms.pdf

  auto k_fact  = static_cast<floating_point_type>(1U);
  auto tk_fact = static_cast<floating_point_type>(2U);
  auto sum     = static_cast<floating_point_type>(static_cast<floating_point_type>(19U) / 18U); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  const floating_point_type lim = std::numeric_limits<floating_point_type>::epsilon();

  for(auto   k = static_cast<std::uint_fast32_t>(UINT32_C(2));
             k < static_cast<std::uint_fast32_t>(UINT32_C(10000000));
           ++k)
  {
    const auto tk                  = static_cast<std::uint32_t>(2U * k);
    const auto tk_plus_one         = static_cast<std::uint32_t>(tk + 1U);
    const auto tk_plus_one_squared = static_cast<std::uint64_t>(static_cast<std::uint64_t>(tk_plus_one) * tk_plus_one);

    k_fact  *= k;
    tk_fact *= static_cast<std::uint64_t>(static_cast<std::uint64_t>(tk) * (tk - 1U));

    floating_point_type term = (k_fact * k_fact) / (tk_fact * tk_plus_one_squared);

    sum += term;

    if(term < lim)
    {
      break;
    }
  }

  const auto result =
    static_cast<floating_point_type>
    (
      static_cast<floating_point_type>
      (
         (
             detail::pi<floating_point_type>()
           * detail::log_two_plus_sqrt_three<floating_point_type>()
         )
       + static_cast<floating_point_type>(sum * 3U)
      ) / 8U
    );

  return result; // NOLINT(performance-no-automatic-move)
}

} // namespace local

auto math::softfloat::example007_catalan_series() -> bool
{
  constexpr auto c = local::catalan<float64_t>();

  using local_representation_type = typename float64_t::representation_type;

  // N[Catalan, 41]
  // 0.9159655941772190150546035149323841107741
  constexpr float64_t
    control
    {
      static_cast<local_representation_type>(UINT64_C(0x3FED4F9713E8135D)),
      math::softfloat::detail::nothing()
    };

  using std::fabs;

  constexpr float64_t closeness = fabs(1 - fabs(c / control));

  constexpr auto tol = static_cast<float64_t>(std::numeric_limits<float64_t>::epsilon() * 10);

  constexpr auto result_is_ok = (closeness < tol);

  #if (defined(SOFT_DOUBLE_CONSTEXPR_IS_COMPILE_TIME_CONST) && (SOFT_DOUBLE_CONSTEXPR_IS_COMPILE_TIME_CONST != 0))
  static_assert(result_is_ok, "Error: example007_catalan_series not OK!");
  #endif

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
