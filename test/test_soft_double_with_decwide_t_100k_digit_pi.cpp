///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <cstdint>
#include <ctime>
#include <limits>
#include <iostream>
#include <random>

#define WIDE_DECIMAL_DISABLE_IOSTREAM
#define WIDE_DECIMAL_DISABLE_DYNAMIC_MEMORY_ALLOCATION
#define WIDE_DECIMAL_DISABLE_CONSTRUCT_FROM_STRING
#define WIDE_DECIMAL_DISABLE_CACHED_CONSTANTS

#include <math/softfloat/soft_double_decwide_t_detail_fft_bindings.h>

#include <math/constants/constants_pi_control_for_decwide_t.h>
#include <math/wide_decimal/decwide_t.h>

void test_soft_double_with_decwide_t_100k_digit_pi_digits10_callback(const std::uint32_t d10)
{
  char p_str[16U] = { 0 };
  char* p_end = util::baselexical_cast(d10, p_str);

  static_cast<void>(p_end);

  std::cout << p_str << std::endl;
}

bool test_soft_double_with_decwide_t_100k_digit_pi();

bool test_soft_double_with_decwide_t_100k_digit_pi()
{
  std::cout << "Start test_soft_double_with_decwide_t_100k_digit_pi()" << std::endl;

  using local_limb_type = std::uint32_t;

  constexpr std::uint32_t wide_decimal_digits10 = UINT32_C(100001);

  constexpr std::int32_t local_elem_digits10 =
    math::wide_decimal::detail::decwide_t_helper<wide_decimal_digits10, local_limb_type>::elem_digits10;

  using local_float_type     = math::softfloat::float64_t;
  using local_float_type_fft = local_float_type;

  using local_wide_decimal_type =
    math::wide_decimal::decwide_t<wide_decimal_digits10,
                                  local_limb_type,
                                  std::allocator<void>,
                                  local_float_type_fft,
                                  std::int32_t,
                                  local_float_type>;

  const std::clock_t start = std::clock();

  const local_wide_decimal_type my_pi =
    math::wide_decimal::pi<wide_decimal_digits10, local_limb_type, std::allocator<void>, local_float_type_fft, std::int32_t, local_float_type>
    (
      test_soft_double_with_decwide_t_100k_digit_pi_digits10_callback
    );

  const std::clock_t stop = std::clock();

  std::cout << "Time test_soft_double_with_decwide_t_100k_digit_pi(): "
            << (float) (stop - start) / (float) CLOCKS_PER_SEC
            << std::endl;

  const bool head_is_ok = std::equal(my_pi.crepresentation().cbegin(),
                                     my_pi.crepresentation().cbegin() + math::constants::const_pi_control_head_32.size(),
                                     math::constants::const_pi_control_head_32.begin());

  using const_iterator_type = typename local_wide_decimal_type::array_type::const_iterator;

  const_iterator_type fi(my_pi.crepresentation().cbegin() + (std::uint32_t) (  (std::uint32_t) (1UL + ((wide_decimal_digits10 - 1UL) / local_elem_digits10))
                                                                             - (std::uint32_t) math::constants::const_pi_control_tail_32_100001.size()));

  const bool tail_is_ok = std::equal(fi,
                                     fi + math::constants::const_pi_control_tail_32_100001.size(),
                                          math::constants::const_pi_control_tail_32_100001.begin());

  const bool result_pi_is_ok = (head_is_ok && tail_is_ok);

  std::cout << "result_pi_is_ok: " << std::boolalpha << result_pi_is_ok << std::endl;

  return result_pi_is_ok;
}
