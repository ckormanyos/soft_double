///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef SOFT_DOUBLE_DECWIDE_T_DETAIL_FFT_BINDINGS_2021_04_02_H
  #define SOFT_DOUBLE_DECWIDE_T_DETAIL_FFT_BINDINGS_2021_04_02_H

  #include <cstdint>

  #include <math/softfloat/soft_double.h>

  WIDE_DECIMAL_NAMESPACE_BEGIN

  #if(__cplusplus >= 201703L)
  namespace math::wide_decimal::detail::fft {
  #else
  namespace math { namespace wide_decimal { namespace detail { namespace fft { // NOLINT(modernize-concat-nested-namespaces)
  #endif

  template<typename float_type>
  constexpr auto template_one() -> float_type;

  template<typename float_type>
  constexpr auto template_half() -> float_type;

  template<typename float_type>
  constexpr auto template_fast_div_by_two(float_type) -> float_type;

  template<typename float_type>
  constexpr auto template_sin_order_1(const std::uint32_t) -> float_type;

  template<>
  constexpr auto template_half<math::softfloat::float64_t>() -> math::softfloat::float64_t
  {
    return math::softfloat::float64_t(UINT64_C(0X3FE0000000000000), math::softfloat::detail::nothing());
  }

  template<>
  constexpr auto template_one<math::softfloat::float64_t>() -> math::softfloat::float64_t
  {
    return math::softfloat::float64_t(UINT64_C(0X3FF0000000000000), math::softfloat::detail::nothing());
  }

  template<>
  constexpr auto template_fast_div_by_two<math::softfloat::float64_t>(math::softfloat::float64_t a) -> math::softfloat::float64_t
  {
    return math::softfloat::float64_t(  (std::uint64_t) (math::softfloat::float64_t::get_rep(a) & ~(std::uint64_t) (0x7FFULL << 52U))
                                      | (std::uint64_t) (((std::uint64_t) ((std::int_fast16_t) ((std::int_fast16_t) ((uint_fast16_t) (math::softfloat::float64_t::get_rep(a) >> 52U) & 0x7FFU) - 1))) << 52U),
                                      math::softfloat::detail::nothing());
  }

  template<>
  constexpr auto template_sin_order_1<math::softfloat::float64_t>(const std::uint32_t num_points) -> math::softfloat::float64_t
  {
    // Table[N[Sin[Pi / (2^n)], 41], {n, 1, 31, 1}]
    // Mathematica command: Table[N[Sin[Pi / (2^n)], 41], {n, 1, 31, 1}]
    return
      (num_points == static_cast<std::uint32_t>(2UL       )) ? math::softfloat::float64_t(UINT64_C(0x3FF0000000000000), math::softfloat::detail::nothing()) : // Pi / 2    : as uint64_t --> UINT64_C(0x3FF0000000000000)
      (num_points == static_cast<std::uint32_t>(4UL       )) ? math::softfloat::float64_t(UINT64_C(0x3FE6A09E667F3BCD), math::softfloat::detail::nothing()) : // Pi / 4    : as uint64_t --> UINT64_C(0x3FE6A09E667F3BCD)
      (num_points == static_cast<std::uint32_t>(8UL       )) ? math::softfloat::float64_t(UINT64_C(0x3FD87DE2A6AEA963), math::softfloat::detail::nothing()) : // Pi / 8    : as uint64_t --> UINT64_C(0x3FD87DE2A6AEA963)
      (num_points == static_cast<std::uint32_t>(16UL      )) ? math::softfloat::float64_t(UINT64_C(0x3FC8F8B83C69A60B), math::softfloat::detail::nothing()) : // Pi / 16   : as uint64_t --> UINT64_C(0x3FC8F8B83C69A60B)
      (num_points == static_cast<std::uint32_t>(32UL      )) ? math::softfloat::float64_t(UINT64_C(0x3FB917A6BC29B42C), math::softfloat::detail::nothing()) : // Pi / 32   : as uint64_t --> UINT64_C(0x3FB917A6BC29B42C)
      (num_points == static_cast<std::uint32_t>(64UL      )) ? math::softfloat::float64_t(UINT64_C(0x3FA91F65F10DD814), math::softfloat::detail::nothing()) : // Pi / 64   : as uint64_t --> UINT64_C(0x3FA91F65F10DD814)
      (num_points == static_cast<std::uint32_t>(128UL     )) ? math::softfloat::float64_t(UINT64_C(0x3F992155F7A3667E), math::softfloat::detail::nothing()) : // Pi / 128  : as uint64_t --> UINT64_C(0x3F992155F7A3667E)
      (num_points == static_cast<std::uint32_t>(1UL <<  8U)) ? math::softfloat::float64_t(UINT64_C(0x3F8921D1FCDEC784), math::softfloat::detail::nothing()) : // Pi / 2^8  : as uint64_t --> UINT64_C(0x3F8921D1FCDEC784)
      (num_points == static_cast<std::uint32_t>(1UL <<  9U)) ? math::softfloat::float64_t(UINT64_C(0x3F7921F0FE670071), math::softfloat::detail::nothing()) : // Pi / 2^9  : as uint64_t --> UINT64_C(0x3F7921F0FE670071)
      (num_points == static_cast<std::uint32_t>(1UL << 10U)) ? math::softfloat::float64_t(UINT64_C(0x3F6921F8BECCA4BA), math::softfloat::detail::nothing()) : // Pi / 2^10 : as uint64_t --> UINT64_C(0x3F6921F8BECCA4BA)
      (num_points == static_cast<std::uint32_t>(1UL << 11U)) ? math::softfloat::float64_t(UINT64_C(0x3F5921FAAEE6472E), math::softfloat::detail::nothing()) : // Pi / 2^11 : as uint64_t --> UINT64_C(0x3F5921FAAEE6472E)
      (num_points == static_cast<std::uint32_t>(1UL << 12U)) ? math::softfloat::float64_t(UINT64_C(0x3F4921FB2AECB360), math::softfloat::detail::nothing()) : // Pi / 2^12 : as uint64_t --> UINT64_C(0x3F4921FB2AECB360)
      (num_points == static_cast<std::uint32_t>(1UL << 13U)) ? math::softfloat::float64_t(UINT64_C(0x3F3921FB49EE4EA6), math::softfloat::detail::nothing()) : // Pi / 2^13 : as uint64_t --> UINT64_C(0x3F3921FB49EE4EA6)
      (num_points == static_cast<std::uint32_t>(1UL << 14U)) ? math::softfloat::float64_t(UINT64_C(0x3F2921FB51AEB57C), math::softfloat::detail::nothing()) : // Pi / 2^14 : as uint64_t --> UINT64_C(0x3F2921FB51AEB57C)
      (num_points == static_cast<std::uint32_t>(1UL << 15U)) ? math::softfloat::float64_t(UINT64_C(0x3F1921FB539ECF31), math::softfloat::detail::nothing()) : // Pi / 2^15 : as uint64_t --> UINT64_C(0x3F1921FB539ECF31)
      (num_points == static_cast<std::uint32_t>(1UL << 16U)) ? math::softfloat::float64_t(UINT64_C(0x3F0921FB541AD59E), math::softfloat::detail::nothing()) : // Pi / 2^16 : as uint64_t --> UINT64_C(0x3F0921FB541AD59E)
      (num_points == static_cast<std::uint32_t>(1UL << 17U)) ? math::softfloat::float64_t(UINT64_C(0x3EF921FB5439D73A), math::softfloat::detail::nothing()) : // Pi / 2^17 : as uint64_t --> UINT64_C(0x3EF921FB5439D73A)
      (num_points == static_cast<std::uint32_t>(1UL << 18U)) ? math::softfloat::float64_t(UINT64_C(0x3EE921FB544197A1), math::softfloat::detail::nothing()) : // Pi / 2^18 : as uint64_t --> UINT64_C(0x3EE921FB544197A1)
      (num_points == static_cast<std::uint32_t>(1UL << 19U)) ? math::softfloat::float64_t(UINT64_C(0x3ED921FB544387BA), math::softfloat::detail::nothing()) : // Pi / 2^19 : as uint64_t --> UINT64_C(0x3ED921FB544387BA)
      (num_points == static_cast<std::uint32_t>(1UL << 20U)) ? math::softfloat::float64_t(UINT64_C(0x3EC921FB544403C1), math::softfloat::detail::nothing()) : // Pi / 2^20 : as uint64_t --> UINT64_C(0x3EC921FB544403C1)
      (num_points == static_cast<std::uint32_t>(1UL << 21U)) ? math::softfloat::float64_t(UINT64_C(0x3EB921FB544422C2), math::softfloat::detail::nothing()) : // Pi / 2^21 : as uint64_t --> UINT64_C(0x3EB921FB544422C2)
      (num_points == static_cast<std::uint32_t>(1UL << 22U)) ? math::softfloat::float64_t(UINT64_C(0x3EA921FB54442A83), math::softfloat::detail::nothing()) : // Pi / 2^22 : as uint64_t --> UINT64_C(0x3EA921FB54442A83)
      (num_points == static_cast<std::uint32_t>(1UL << 23U)) ? math::softfloat::float64_t(UINT64_C(0x3E9921FB54442C73), math::softfloat::detail::nothing()) : // Pi / 2^23 : as uint64_t --> UINT64_C(0x3E9921FB54442C73)
      (num_points == static_cast<std::uint32_t>(1UL << 24U)) ? math::softfloat::float64_t(UINT64_C(0x3E8921FB54442CEF), math::softfloat::detail::nothing()) : // Pi / 2^24 : as uint64_t --> UINT64_C(0x3E8921FB54442CEF)
      (num_points == static_cast<std::uint32_t>(1UL << 25U)) ? math::softfloat::float64_t(UINT64_C(0x3E7921FB54442D0E), math::softfloat::detail::nothing()) : // Pi / 2^25 : as uint64_t --> UINT64_C(0x3E7921FB54442D0E)
      (num_points == static_cast<std::uint32_t>(1UL << 26U)) ? math::softfloat::float64_t(UINT64_C(0x3E6921FB54442D16), math::softfloat::detail::nothing()) : // Pi / 2^26 : as uint64_t --> UINT64_C(0x3E6921FB54442D16)
      (num_points == static_cast<std::uint32_t>(1UL << 27U)) ? math::softfloat::float64_t(UINT64_C(0x3E5921FB54442D18), math::softfloat::detail::nothing()) : // Pi / 2^27 : as uint64_t --> UINT64_C(0x3E5921FB54442D18)
      (num_points == static_cast<std::uint32_t>(1UL << 28U)) ? math::softfloat::float64_t(UINT64_C(0x3E4921FB54442D18), math::softfloat::detail::nothing()) : // Pi / 2^28 : as uint64_t --> UINT64_C(0x3E4921FB54442D18)
      (num_points == static_cast<std::uint32_t>(1UL << 29U)) ? math::softfloat::float64_t(UINT64_C(0x3E3921FB54442D18), math::softfloat::detail::nothing()) : // Pi / 2^29 : as uint64_t --> UINT64_C(0x3E3921FB54442D18)
      (num_points == static_cast<std::uint32_t>(1UL << 30U)) ? math::softfloat::float64_t(UINT64_C(0x3E2921FB54442D18), math::softfloat::detail::nothing()) : // Pi / 2^30 : as uint64_t --> UINT64_C(0x3E2921FB54442D18)
      (num_points == static_cast<std::uint32_t>(1UL << 31U)) ? math::softfloat::float64_t(UINT64_C(0x3E1921FB54442D18), math::softfloat::detail::nothing()) : // Pi / 2^31 : as uint64_t --> UINT64_C(0x3E1921FB54442D18)
                                                               math::softfloat::float64_t(UINT64_C(0x0000000000000000), math::softfloat::detail::nothing())   // Pi        : as uint64_t --> UINT64_C(0x0000000000000000)
      ;
  }

  #if(__cplusplus >= 201703L)
  } // namespace math::wide_decimal::detail::fft
  #else
  } // namespace fft
  } // namespace detail
  } // namespace wide_decimal
  } // namespace math
  #endif

  WIDE_DECIMAL_NAMESPACE_END

#endif // SOFT_DOUBLE_DECWIDE_T_DETAIL_FFT_BINDINGS_2021_04_02_H
