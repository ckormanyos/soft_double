///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2022.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include <math/softfloat/soft_double.h>

namespace test_soft_double_edge {

using eng_sgn_type = std::ranlux24;
using eng_dig_type = std::mt19937;
using eng_d15_type = std::mt19937_64;
using eng_pow_type = std::ranlux48;

using distribution32_type = std::uniform_int_distribution<std::uint32_t>;
using distribution64_type = std::uniform_int_distribution<std::uint64_t>;

distribution32_type dist_sgn(static_cast<std::uint32_t>(UINT32_C(0)), static_cast<std::uint32_t>(UINT32_C(1)));               // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
distribution32_type dist_top(static_cast<std::uint32_t>(UINT32_C(1)), static_cast<std::uint32_t>(UINT32_C(99999999)));        // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
distribution32_type dist_bot(static_cast<std::uint32_t>(UINT32_C(1)), static_cast<std::uint32_t>(UINT32_C(99999999)));        // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
distribution32_type dist_pow(static_cast<std::uint32_t>(UINT32_C(2)), static_cast<std::uint32_t>(UINT32_C(7)));               // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
distribution64_type dist_d15(static_cast<std::uint64_t>(UINT64_C(1)), static_cast<std::uint64_t>(UINT64_C(999999999999999))); // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)

eng_sgn_type eng_sgn; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
eng_dig_type eng_top; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
eng_dig_type eng_bot; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
eng_pow_type eng_pow; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
eng_d15_type eng_d15; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)

template<typename IntegralTimePointType,
         typename ClockType = std::chrono::high_resolution_clock>
auto time_point() -> IntegralTimePointType
{
  using local_integral_time_point_type = IntegralTimePointType;
  using local_clock_type               = ClockType;

  const auto current_now =
    static_cast<std::uintmax_t>
    (
      std::chrono::duration_cast<std::chrono::nanoseconds>
      (
        local_clock_type::now().time_since_epoch()
      ).count()
    );

  return static_cast<local_integral_time_point_type>(current_now);
}

auto generate_soft_double_value(bool is_positive = true) -> ::math::softfloat::float64_t
{
  using ::math::softfloat::float64_t;

  auto result = float64_t::my_value_pi();

  result *= float64_t(dist_top(eng_top));
  result /= float64_t(dist_bot(eng_bot));

  if(!is_positive)
  {
    const auto is_neg =
      (dist_sgn(eng_sgn) != static_cast<typename distribution32_type::result_type>(UINT8_C(0)));

    if(is_neg)
    {
      result = -result;
    }
  }

  return result;
}

auto test_various_functions() -> bool
{
  auto result_is_ok = true;

  using ::math::softfloat::float64_t;

  {
    const auto val_one = float64_t::my_value_one();

    using std::log;

    const auto val_ln_one = log(val_one);

    const auto result_val_ln_one_is_ok = (val_ln_one == float64_t::my_value_zero());

    result_is_ok = (result_val_ln_one_is_ok && result_is_ok);
  }

  {
    const auto val_one = float64_t::my_value_one();

    using std::sin;

    const auto val_sin_1m = sin(val_one * -1);
    const auto val_sin_2m = sin(val_one * -2);
    const auto val_sin_3m = sin(val_one * -3);

    const auto val_sin_1p = sin(val_one * +1);
    const auto val_sin_2p = sin(val_one * +2);
    const auto val_sin_3p = sin(val_one * +3);

    const auto result_sin_m_is_ok =
    (
         val_sin_1m == -val_sin_1p
      && val_sin_2m == -val_sin_2p
      && val_sin_3m == -val_sin_3p
    );

    result_is_ok = (result_sin_m_is_ok && result_is_ok);
  }

  {
    using small_values_array_type = std::array<float64_t, static_cast<std::size_t>(UINT8_C(7))>;

    const small_values_array_type small_values =
    {
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-001).my_u, ::math::softfloat::detail::nothing() },
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-051).my_u, ::math::softfloat::detail::nothing() },
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-101).my_u, ::math::softfloat::detail::nothing() },
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-151).my_u, ::math::softfloat::detail::nothing() },
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-201).my_u, ::math::softfloat::detail::nothing() },
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-251).my_u, ::math::softfloat::detail::nothing() },
      float64_t { ::math::softfloat::detail::uz_type<double>(1.0E-301).my_u, ::math::softfloat::detail::nothing() }
    };

    for(const auto& soft_double_elem : small_values)
    {
      const auto u_zero = static_cast<std::uint64_t>(soft_double_elem);
      const auto n_zero = static_cast<std::int64_t> (soft_double_elem);

      const auto result_round_to_zero_is_ok =
      (
           (u_zero == static_cast<std::uint64_t>(UINT8_C(0)))
        && (n_zero == static_cast<std::int64_t> ( INT8_C(0)))
      );

      result_is_ok = (result_round_to_zero_is_ok && result_is_ok);
    }
  }

  return result_is_ok;
}

auto test_various_ostream_ops() -> bool
{
  auto result_is_ok = true;

  {
    eng_sgn.seed(time_point<typename eng_sgn_type::result_type>());
    eng_top.seed(time_point<typename eng_dig_type::result_type>());
    eng_bot.seed(time_point<typename eng_dig_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(100000));
             ++i)
    {
      const auto value_as_soft_double     = generate_soft_double_value(false);
      const auto value_as_built_in_double = ::math::softfloat::detail::uz_type<double>(value_as_soft_double.crepresentation());

      std::stringstream strm_for_soft_double;
      std::stringstream strm_for_built_in_double;

      strm_for_soft_double     << value_as_soft_double;
      strm_for_built_in_double << value_as_built_in_double.my_f; // NOLINT(cppcoreguidelines-pro-type-union-access)

      const auto result_write_soft_double_is_ok = (strm_for_soft_double.str() == strm_for_built_in_double.str());

      result_is_ok = (result_write_soft_double_is_ok && result_is_ok);
    }
  }

  {
    eng_sgn.seed(time_point<typename eng_sgn_type::result_type>());
    eng_top.seed(time_point<typename eng_dig_type::result_type>());
    eng_bot.seed(time_point<typename eng_dig_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(100000));
             ++i)
    {
      const auto value_as_soft_double     = generate_soft_double_value(false);
      const auto value_as_built_in_double = math::softfloat::detail::uz_type<double>(value_as_soft_double.crepresentation());

      std::stringstream strm_for_soft_double;
      std::stringstream strm_for_built_in_double;

      strm_for_soft_double     << std::scientific << value_as_soft_double;
      strm_for_built_in_double << std::scientific << value_as_built_in_double.my_f; // NOLINT(cppcoreguidelines-pro-type-union-access)

      const auto result_write_soft_double_is_ok = (strm_for_soft_double.str() == strm_for_built_in_double.str());

      result_is_ok = (result_write_soft_double_is_ok && result_is_ok);
    }
  }

  return result_is_ok;
}

auto test_various_pos_powers() -> bool
{
  auto result_is_ok = true;

  {
    eng_sgn.seed(time_point<typename eng_sgn_type::result_type>());
    eng_top.seed(time_point<typename eng_dig_type::result_type>());
    eng_bot.seed(time_point<typename eng_dig_type::result_type>());
    eng_pow.seed(time_point<typename eng_pow_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(10000));
             ++i)
    {
      const auto arg_as_soft_double     = generate_soft_double_value(false);
      const auto arg_as_built_in_double = math::softfloat::detail::uz_type<double>(arg_as_soft_double.crepresentation());

      const auto pos_power = dist_pow(eng_pow);

      using std::pow;

      const auto pow_as_soft_double     = pow(arg_as_soft_double,          pos_power);
      const auto pow_as_built_in_double = pow(arg_as_built_in_double.my_f, pos_power); // NOLINT(cppcoreguidelines-pro-type-union-access)

      std::stringstream strm_for_soft_double;
      std::stringstream strm_for_built_in_double;

      strm_for_soft_double     << pow_as_soft_double;
      strm_for_built_in_double << pow_as_built_in_double;

      const auto result_pos_power_of_soft_double_is_ok = (strm_for_soft_double.str() == strm_for_built_in_double.str());

      result_is_ok = (result_pos_power_of_soft_double_is_ok && result_is_ok);
    }

    return result_is_ok;
  }
}

auto test_various_64_bit_cast() -> bool
{
  auto result_is_ok = true;

  using ::math::softfloat::float64_t;

  {
    eng_sgn.seed(time_point<typename eng_sgn_type::result_type>());
    eng_d15.seed(time_point<typename eng_d15_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(100000));
             ++i)
    {
      const auto is_neg =
      (
        dist_sgn(eng_sgn) != static_cast<typename distribution32_type::result_type>(UINT8_C(0))
      );

      const auto n15_as_i64 =
        static_cast<std::int64_t>
        (
          (!is_neg) ? +static_cast<std::int64_t>(dist_d15(eng_d15))
                    : -static_cast<std::int64_t>(dist_d15(eng_d15))
        );

      const auto value_as_soft_double = float64_t(n15_as_i64);

      const auto result_cast_soft_double_is_ok = (static_cast<std::int64_t>(value_as_soft_double) == n15_as_i64);

      result_is_ok = (result_cast_soft_double_is_ok && result_is_ok);
    }
  }

  {
    eng_d15.seed(time_point<typename eng_d15_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(100000));
             ++i)
    {
      const auto u15_as_ui64 = dist_d15(eng_d15);

      const auto value_as_soft_double = float64_t(u15_as_ui64);

      const auto result_cast_soft_double_is_ok = (static_cast<std::uint64_t>(value_as_soft_double) == u15_as_ui64);

      result_is_ok = (result_cast_soft_double_is_ok && result_is_ok);
    }
  }

  return result_is_ok;
}

} // namespace test_soft_double_edge

auto test_soft_double_edge_cases() -> bool
{
  auto result_edge_cases_is_ok = true;

  std::cout << "test_soft_double_edge_cases          : ";

  result_edge_cases_is_ok = (test_soft_double_edge::test_various_functions  () && result_edge_cases_is_ok);
  result_edge_cases_is_ok = (test_soft_double_edge::test_various_ostream_ops() && result_edge_cases_is_ok);
  result_edge_cases_is_ok = (test_soft_double_edge::test_various_pos_powers () && result_edge_cases_is_ok);
  result_edge_cases_is_ok = (test_soft_double_edge::test_various_64_bit_cast() && result_edge_cases_is_ok);

  std::cout << std::boolalpha << result_edge_cases_is_ok << std::endl;

  return result_edge_cases_is_ok;
}
