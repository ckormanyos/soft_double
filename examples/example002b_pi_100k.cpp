///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <limits>
#include <random>

#define WIDE_DECIMAL_DISABLE_IOSTREAM
#define WIDE_DECIMAL_DISABLE_DYNAMIC_MEMORY_ALLOCATION
#define WIDE_DECIMAL_DISABLE_CONSTRUCT_FROM_STRING
#define WIDE_DECIMAL_DISABLE_CACHED_CONSTANTS

#include <math/constants/constants_pi_control_for_decwide_t.h>
#include <math/softfloat/soft_double_decwide_t_detail_fft_bindings.h>
#include <math/softfloat/soft_double_examples.h>
#include <math/wide_decimal/decwide_t.h>

namespace local
{
  auto example002b_pi_100k_digits10_callback(const std::uint32_t d10) -> void
  {
    using char_array_type = std::array<char, static_cast<std::size_t>(UINT8_C(16))>;

    char_array_type p_str { };
    p_str.fill('\0');

    char* p_end = util::baselexical_cast(d10, p_str.data());

    static_cast<void>(p_end);

    std::cout << p_str.data() << std::endl;
  }
} // namespace local

auto math::softfloat::example002b_pi_100k() -> bool
{
  std::cout << "Start example002b_pi_100k()" << std::endl;

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
      local::example002b_pi_100k_digits10_callback
    );

  const std::clock_t stop = std::clock();

  std::cout << "Time example002b_pi_100k(): "
            << static_cast<float>(stop - start) / static_cast<float>(CLOCKS_PER_SEC)
            << std::endl;

  const bool head_is_ok = std::equal(my_pi.crepresentation().cbegin(),
                                     my_pi.crepresentation().cbegin() + math::constants::const_pi_control_head_32.size(),
                                     math::constants::const_pi_control_head_32.begin());

  using const_iterator_type = typename local_wide_decimal_type::representation_type::const_iterator;

  const_iterator_type
    fi
    (
        my_pi.crepresentation().cbegin()
      + static_cast<std::uint32_t>(  static_cast<std::uint32_t>(1UL + ((wide_decimal_digits10 - 1UL) / local_elem_digits10))
                                   - static_cast<std::uint32_t>(math::constants::const_pi_control_tail_32_100001.size()))
    );

  const bool tail_is_ok = std::equal(fi,
                                     fi + math::constants::const_pi_control_tail_32_100001.size(),
                                          math::constants::const_pi_control_tail_32_100001.begin());

  const bool result_pi_is_ok = (head_is_ok && tail_is_ok);

  {
    const auto flg = std::cout.flags();

    std::cout << "result_pi_is_ok                      : " << std::boolalpha << result_pi_is_ok << std::endl;

    std::cout.flags(flg);
  }

  return result_pi_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = math::softfloat::example002b_pi_100k();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
