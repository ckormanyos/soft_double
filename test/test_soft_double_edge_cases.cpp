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
using eng_dig_type = std::ranlux48;
using distribution_type = std::uniform_int_distribution<std::uint32_t>;

distribution_type dist_sgn  (static_cast<std::uint32_t>(UINT32_C(0)), static_cast<std::uint32_t>(UINT32_C(1)));         // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
distribution_type dist_numer(static_cast<std::uint32_t>(UINT32_C(1)), static_cast<std::uint32_t>(UINT32_C(99999999)));  // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
distribution_type dist_denom(static_cast<std::uint32_t>(UINT32_C(1)), static_cast<std::uint32_t>(UINT32_C(99999999)));  // NOLINT(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)

eng_sgn_type eng_sgn;   // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
eng_dig_type eng_numer; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
eng_dig_type eng_denom; // NOLINT(cert-msc32-c,cert-msc51-cpp,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)

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

  result *= float64_t(dist_numer(eng_numer));
  result /= float64_t(dist_denom(eng_denom));

  if(!is_positive)
  {
    const auto is_neg =
      (dist_sgn(eng_sgn) != static_cast<typename distribution_type::result_type>(UINT8_C(0)));

    if(is_neg)
    {
      result = -result;
    }
  }

  return result;
}

auto test_various_ostream_ops() -> bool
{
  auto result_is_ok = true;

  {
    eng_sgn.seed  (time_point<typename eng_sgn_type::result_type>());
    eng_numer.seed(time_point<typename eng_dig_type::result_type>());
    eng_denom.seed(time_point<typename eng_dig_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(100000));
             ++i)
    {
      const auto value_as_soft_double     = generate_soft_double_value(false);
      const auto value_as_built_in_double = math::softfloat::detail::uz_type<double>(value_as_soft_double.crepresentation());

      std::stringstream strm_for_soft_double;
      std::stringstream strm_for_built_in_double;

      strm_for_soft_double     << value_as_soft_double;
      strm_for_built_in_double << value_as_built_in_double.my_f;

      const auto result_write_soft_double_is_ok = (strm_for_soft_double.str() == strm_for_built_in_double.str());

      result_is_ok = (result_write_soft_double_is_ok && result_is_ok);
    }
  }

  {
    eng_sgn.seed  (time_point<typename eng_sgn_type::result_type>());
    eng_numer.seed(time_point<typename eng_dig_type::result_type>());
    eng_denom.seed(time_point<typename eng_dig_type::result_type>());

    for(auto   i = static_cast<std::uint32_t>(UINT8_C(0));
               i < static_cast<std::uint32_t>(UINT32_C(100000));
             ++i)
    {
      const auto value_as_soft_double     = generate_soft_double_value(false);
      const auto value_as_built_in_double = math::softfloat::detail::uz_type<double>(value_as_soft_double.crepresentation());

      std::stringstream strm_for_soft_double;
      std::stringstream strm_for_built_in_double;

      strm_for_soft_double     << std::scientific << value_as_soft_double;
      strm_for_built_in_double << std::scientific << value_as_built_in_double.my_f;

      const auto result_write_soft_double_is_ok = (strm_for_soft_double.str() == strm_for_built_in_double.str());

      result_is_ok = (result_write_soft_double_is_ok && result_is_ok);
    }
  }

  return result_is_ok;
}

} // namespace test_soft_double_edge

auto test_soft_double_edge_cases() -> bool
{
  auto result_edge_cases_is_ok = true;

  std::cout << "test_soft_double_edge_cases():    ";

  result_edge_cases_is_ok = (test_soft_double_edge::test_various_ostream_ops() && result_edge_cases_is_ok);

  std::cout << std::boolalpha << result_edge_cases_is_ok << std::endl;

  return result_edge_cases_is_ok;
}
