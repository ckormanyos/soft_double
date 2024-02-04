///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2022 - 2024.                  //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <iomanip>
#include <iostream>

#include <cmath>

#include <math/softfloat/soft_double.h>

namespace detail
{
  template<typename NumericType>
  auto is_close_fraction(const NumericType a,
                         const NumericType b, // NOLINT(bugprone-easily-swappable-parameters)
                         const NumericType tol = static_cast<NumericType>(std::numeric_limits<NumericType>::epsilon() * static_cast<NumericType>(100))) -> bool
  {
    using std::fabs;

    const auto ratio = fabs(static_cast<NumericType>((static_cast<NumericType>(1) * a) / b));

    const auto closeness = fabs(static_cast<NumericType>(static_cast<NumericType>(1) - ratio));

    return (closeness < tol);
  }

} // namespace detail

namespace local
{
  auto test_constexpr_init_issue110() -> bool
  {
    using ::math::softfloat::float64_t;

    #if ((defined SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS) && (SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS == 1))
    constexpr auto gravitational_constant = float64_t { 6.674e-11 };
    constexpr auto near_pi_constant       = float64_t { 3.14 };
    constexpr auto one_quarter_constant   = float64_t { 0.25 };
    #else
    const auto gravitational_constant = float64_t { 6.674e-11 };
    const auto near_pi_constant       = float64_t { 3.14 };
    const auto one_quarter_constant   = float64_t { 0.25 };
    #endif

    #if ((defined SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS) && (SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS == 1))
    static_assert(gravitational_constant < 1, "Error: Initialization constexpr-double does not properly work");
    static_assert(gravitational_constant != near_pi_constant, "Error: Initialization constexpr-double does not properly work");
    static_assert(4 * one_quarter_constant == 1, "Error: Initialization constexpr-double does not properly work");
    static_assert(static_cast<int>(INT8_C(12)) * one_quarter_constant < near_pi_constant, "Error: Initialization constexpr-double does not properly work");
    static_assert(static_cast<int>(INT8_C(13)) * one_quarter_constant > near_pi_constant, "Error: Initialization constexpr-double does not properly work");
    #endif

    bool result_is_ok { true };

    const auto result_check_00_is_ok = (gravitational_constant < 1);
    result_is_ok = (result_check_00_is_ok && result_is_ok);

    const auto result_check_01_is_ok = (gravitational_constant != near_pi_constant);
    result_is_ok = (result_check_01_is_ok && result_is_ok);

    const auto result_check_02_is_ok = (4 * one_quarter_constant == 1);
    result_is_ok = (result_check_02_is_ok && result_is_ok);

    const auto result_check_03_is_ok = (12 * one_quarter_constant < near_pi_constant);
    result_is_ok = (result_check_03_is_ok && result_is_ok);

    const auto result_check_04_is_ok = (13 * one_quarter_constant > near_pi_constant);
    result_is_ok = (result_check_04_is_ok && result_is_ok);

    return result_is_ok;
  }

  auto test_construct_from_float() -> bool
  {
    constexpr auto f0 = static_cast<float>(0.125L);
    constexpr auto f1 = static_cast<float>(0.25L);
    constexpr auto f2 = static_cast<float>(0.5L);
    constexpr auto f3 = static_cast<float>(0.625L);

    constexpr auto ctrl_d0 = ::math::softfloat::float64_t { static_cast<int>(INT8_C(1)) } / static_cast<int>(INT8_C(8));
    constexpr auto ctrl_d1 = ::math::softfloat::float64_t { static_cast<int>(INT8_C(1)) } / static_cast<int>(INT8_C(4));
    constexpr auto ctrl_d2 = ::math::softfloat::float64_t { static_cast<int>(INT8_C(1)) } / static_cast<int>(INT8_C(2));
    constexpr auto ctrl_d3 = ::math::softfloat::float64_t { static_cast<int>(INT8_C(5)) } / static_cast<int>(INT8_C(8));

    #if ((defined SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS) && (SOFT_DOUBLE_CONSTEXPR_BUILTIN_FLOATS == 1))
    constexpr auto sd0 = ::math::softfloat::float64_t { f0 };
    constexpr auto sd1 = ::math::softfloat::float64_t { f1 };
    constexpr auto sd2 = ::math::softfloat::float64_t { f2 };
    constexpr auto sd3 = ::math::softfloat::float64_t { f3 };

    static_assert(sd0 == ctrl_d0, "Error: Initialization from constexpr-float does not properly work");
    static_assert(sd1 == ctrl_d1, "Error: Initialization from constexpr-float does not properly work");
    static_assert(sd2 == ctrl_d2, "Error: Initialization from constexpr-float does not properly work");
    static_assert(sd3 == ctrl_d3, "Error: Initialization from constexpr-float does not properly work");
    #else
    constexpr auto sd0 = ::math::softfloat::float64_t { f0 };
    constexpr auto sd1 = ::math::softfloat::float64_t { f1 };
    constexpr auto sd2 = ::math::softfloat::float64_t { f2 };
    constexpr auto sd3 = ::math::softfloat::float64_t { f3 };
    #endif

    auto result_construct_from_float_is_ok =
    (
         (sd0 == ctrl_d0)
      && (sd1 == ctrl_d1)
      && (sd2 == ctrl_d2)
      && (sd3 == ctrl_d3)
    );

    {
      const auto one_sixth = sd2 / static_cast<::math::softfloat::float64_t>(static_cast<float>(3.0L));

      const auto result_one_sixth_is_not_one_half_is_ok = (one_sixth != sd2);

      const auto result_one_sixth_is_ok =
        detail::is_close_fraction
        (
          one_sixth,
          static_cast<::math::softfloat::float64_t>(static_cast<unsigned>(UINT8_C(1))) / static_cast<unsigned>(UINT8_C(6))
        );

      result_construct_from_float_is_ok =
      (
           result_one_sixth_is_not_one_half_is_ok
        && result_one_sixth_is_ok
        && result_construct_from_float_is_ok
      );
    }

    return result_construct_from_float_is_ok;
  }

  auto test_hypergeometric_trig() -> bool
  {
    const auto x_half = ::math::softfloat::float64_t { static_cast<int>(INT8_C(1)) } / static_cast<int>(INT8_C(2));

    const auto sinh_x_val = sinh(x_half);
    const auto cosh_x_val = cosh(x_half);
    const auto tanh_x_val = tanh(x_half);

    // N[Sinh[1/2], 20]
    // 0.52109530549374736162
    const auto sinh_x_ctrl = static_cast<::math::softfloat::float64_t>(static_cast<double>(0.52109530549374736162L));

    // N[Cosh[1/2], 20]
    // 1.1276259652063807852
    const auto cosh_x_ctrl = static_cast<::math::softfloat::float64_t>(static_cast<double>(1.1276259652063807852L));

    // N[Tanh[1/2], 20]
    // 0.46211715726000975850
    const auto tanh_x_ctrl = static_cast<::math::softfloat::float64_t>(static_cast<double>(0.46211715726000975850L));

    const auto tol_sinh_cosh = std::numeric_limits<::math::softfloat::float64_t>::epsilon() * static_cast<::math::softfloat::float64_t>(static_cast<double>(80.0L));
    const auto tol_tanh      = std::numeric_limits<::math::softfloat::float64_t>::epsilon() * static_cast<::math::softfloat::float64_t>(static_cast<double>(80.0L));

    const auto result_sinh_is_ok = detail::is_close_fraction(sinh_x_val, sinh_x_ctrl, tol_sinh_cosh);
    const auto result_cosh_is_ok = detail::is_close_fraction(cosh_x_val, cosh_x_ctrl, tol_sinh_cosh);
    const auto result_tanh_is_ok = detail::is_close_fraction(tanh_x_val, tanh_x_ctrl, tol_tanh);

    const auto result_hypergeometric_trig_is_ok = (result_sinh_is_ok && result_cosh_is_ok && result_tanh_is_ok);

    return result_hypergeometric_trig_is_ok;
  }

  auto test_sin_near_pi_half() -> bool
  {
    using control_value_array_type = std::array<::math::softfloat::float64_t, static_cast<std::size_t>(UINT8_C(5))>;

    // Table[N[Sin[(Pi/2) - (Pi/(10 + n))], 20], {n, 0, 4, 1}]

    const auto control_values =
      control_value_array_type
      {
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.95105651629515357212L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.95949297361449738989L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.96592582628906828675L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.97094181742605202716L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.97492791218182360702L))
      };

    auto result_sin_near_pi_is_ok = true;

    const auto tol_sin = std::numeric_limits<::math::softfloat::float64_t>::epsilon() * static_cast<::math::softfloat::float64_t>(static_cast<double>(60.0L));

    for(auto   i = static_cast<std::size_t>(UINT8_C(0));
               i < std::tuple_size<control_value_array_type>::value;
             ++i)
    {
      const auto x =
        static_cast<::math::softfloat::float64_t>
        (
            ::math::softfloat::float64_t::my_value_pi_half()
          - ::math::softfloat::float64_t::my_value_pi() / (::math::softfloat::float64_t(static_cast<unsigned>(UINT8_C(10))) + i)
        );

      const auto sin_x = sin(x);

      const auto result_sin_is_ok = detail::is_close_fraction(sin_x, control_values.at(i), tol_sin);

      result_sin_near_pi_is_ok = (result_sin_is_ok && result_sin_near_pi_is_ok);
    }

    return result_sin_near_pi_is_ok;
  }

  auto test_pow() -> bool
  {
    using control_value_array_type = std::array<::math::softfloat::float64_t, static_cast<std::size_t>(UINT8_C(5))>;

    // Table[N[((12 + i)/10)^(456/100), 20], {i, 0, 4, 1}]

    const auto control_values =
      control_value_array_type
      {
        static_cast<::math::softfloat::float64_t>(static_cast<double>(2.2965001703927784205L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(3.3081232875218951210L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(4.6381398849287209722L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(6.3529602982502129259L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(8.5268205051338967808L))
      };

    auto result_pow_is_ok = true;

    const auto tol_pow = std::numeric_limits<::math::softfloat::float64_t>::epsilon() * static_cast<::math::softfloat::float64_t>(static_cast<double>(120.0L));

    for(auto   i = static_cast<std::size_t>(UINT8_C(0));
               i < std::tuple_size<control_value_array_type>::value;
             ++i)
    {
      const auto x =
        static_cast<::math::softfloat::float64_t>
        (
            (::math::softfloat::float64_t(static_cast<unsigned>(UINT8_C(12))) + i)
          / static_cast<unsigned>(UINT8_C(10))
        );

      const auto a =
        static_cast<::math::softfloat::float64_t>
        (
            ::math::softfloat::float64_t(static_cast<unsigned>(UINT16_C(456)))
          / static_cast<unsigned>(UINT8_C(100))
        );

      const auto x_pow_a = pow(x, a);

      const auto result_x_pow_a_is_ok = detail::is_close_fraction(x_pow_a, control_values.at(i), tol_pow);

      result_pow_is_ok = (result_x_pow_a_is_ok && result_pow_is_ok);
    }

    return result_pow_is_ok;
  }

  auto test_tan() -> bool
  {
    using control_value_array_type = std::array<::math::softfloat::float64_t, static_cast<std::size_t>(UINT8_C(9))>;

    // Table[N[Tan[(Pi/4) + ((i - 4)/10)], 20], {i, 0, 8, 1}]

    const auto control_values =
      control_value_array_type
      {
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.40568564262187578775L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.52749150617062438754L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.66291120964507694113L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(0.81762880943252020647L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(1.0000000000000000000L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(1.2230488804498651731L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(1.5084976471214004651L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(1.8957651228540090217L)),
        static_cast<::math::softfloat::float64_t>(static_cast<double>(2.4649627567226038132L))
      };

    auto result_tan_is_ok = true;

    const auto tol_tan = std::numeric_limits<::math::softfloat::float64_t>::epsilon() * static_cast<::math::softfloat::float64_t>(static_cast<double>(120.0L));

    for(auto   i = static_cast<std::size_t>(UINT8_C(0));
               i < std::tuple_size<control_value_array_type>::value;
             ++i)
    {
      const auto x =
        static_cast<::math::softfloat::float64_t>
        (
            (::math::softfloat::float64_t::my_value_pi() / static_cast<unsigned>(UINT8_C(4)))
          +
            static_cast<::math::softfloat::float64_t>
            (
                static_cast<::math::softfloat::float64_t>(static_cast<int>(i) - static_cast<int>(INT8_C(4)))
              / static_cast<unsigned>(UINT8_C(10))
            )
        );

      const auto tan_x = tan(x);

      const auto result_tan_x_is_ok = detail::is_close_fraction(tan_x, control_values.at(i), tol_tan);

      result_tan_is_ok = (result_tan_x_is_ok && result_tan_is_ok);
    }

    return result_tan_is_ok;
  }
} // namespace local

auto test_soft_double_spot_values() -> bool
{
  auto result_spot_values_is_ok = true;

  {
    const auto result_constexpr_init_issue110_is_ok   = local::test_constexpr_init_issue110();

    result_spot_values_is_ok = (result_constexpr_init_issue110_is_ok && result_spot_values_is_ok);
  }

  {
    const auto result_construct_from_float_is_ok   = local::test_construct_from_float();

    result_spot_values_is_ok = (result_construct_from_float_is_ok && result_spot_values_is_ok);
  }

  {
    const auto result_hypergeometric_trig_is_ok = local::test_hypergeometric_trig();

    result_spot_values_is_ok = (result_hypergeometric_trig_is_ok && result_spot_values_is_ok);
  }

  {
    const auto result_sin_near_pi_half = local::test_sin_near_pi_half();

    result_spot_values_is_ok = (result_sin_near_pi_half && result_spot_values_is_ok);
  }

  {
    const auto result_pow = local::test_pow();

    result_spot_values_is_ok = (result_pow && result_spot_values_is_ok);
  }

  {
    const auto result_tan = local::test_tan();

    result_spot_values_is_ok = (result_tan && result_spot_values_is_ok);
  }

  return result_spot_values_is_ok;
}
