///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

// This work uses (significantly) modified parts of
// SoftFloat IEEE Floating-Point Arithmetic Package,
// Release 3e, by John R. Hauser.

// Full original copyright information follows.
/*----------------------------------------------------------------------------

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-----------------------------------------------------------------------------*/

#ifndef SOFT_DOUBLE_2020_10_27_H
  #define SOFT_DOUBLE_2020_10_27_H

  //#define SOFT_DOUBLE_DISABLE_IOSTREAM

  #include <array>
  #include <cstdint>
  #include <cstddef>
  #if !defined(SOFT_DOUBLE_DISABLE_IOSTREAM)
  #include <iomanip>
  #include <sstream>
  #endif
  #include <limits>
  #include <type_traits>

  #if(__cplusplus >= 201703L)
  namespace math::softfloat {
  #else
  namespace math { namespace softfloat { // NOLINT(modernize-concat-nested-namespaces)
  #endif

  // Forward declaration the math::softfloat::soft_double class.
  class soft_double;

  #if(__cplusplus >= 201703L)
  } // namespace math::softfloat
  #else
  } // namespace softfloat
  } // namespace math
  #endif

  namespace std {

  // Forward declaration of the specialization of numeric_limits for soft_double.
  template<>
  class numeric_limits<math::softfloat::soft_double>;

  } // namespace std

  #if(__cplusplus >= 201703L)
  namespace math::softfloat {
  #else
  namespace math { namespace softfloat { // NOLINT(modernize-concat-nested-namespaces)
  #endif

  namespace detail {

  struct uint128_as_struct { std::uint64_t v0;    std::uint64_t v64; };
  struct uint64_extra      { std::uint64_t extra; std::uint64_t v; };

  template<typename UnsignedIntegralType>
  constexpr auto negate(UnsignedIntegralType u) -> typename std::enable_if<   (std::is_integral<UnsignedIntegralType>::value == true)
                                                                           && (std::is_unsigned<UnsignedIntegralType>::value == true), UnsignedIntegralType>::type
  {
    return
      static_cast<UnsignedIntegralType>
      (
          static_cast<UnsignedIntegralType>(~u)
        + static_cast<UnsignedIntegralType>(UINT8_C(1))
      );
  }

  template<typename SignedIntegralType>
  constexpr auto negate(SignedIntegralType n) -> typename std::enable_if<   (std::is_integral<SignedIntegralType>::value == true)
                                                                         && (std::is_signed  <SignedIntegralType>::value == true), SignedIntegralType>::type
  {
    return static_cast<SignedIntegralType>(detail::negate(static_cast<unsigned long long>(n)));
  }

  inline constexpr auto signF32UI(std::uint32_t a) -> bool          { return (static_cast<unsigned>(a >> 31U) != static_cast<unsigned>(UINT8_C(0))); }
  inline constexpr auto expF32UI (std::uint32_t a) -> std::int16_t  { return static_cast<std::int16_t>(static_cast<std::int16_t>(a >> 23U) & static_cast<std::int16_t>(INT16_C(0xFF))); }
  inline constexpr auto fracF32UI(std::uint32_t a) -> std::uint32_t { return static_cast<std::uint32_t>(a & static_cast<std::uint32_t>(UINT32_C(0x007FFFFF))); }

  inline constexpr auto signF64UI(std::uint64_t a) -> bool          { return (static_cast<unsigned>(a >> 63U) != static_cast<unsigned>(UINT8_C(0))); }
  inline constexpr auto expF64UI (std::uint64_t a) -> std::int16_t  { return static_cast<std::int16_t>(static_cast<std::int16_t>(a >> 52U) & static_cast<std::int16_t>(INT16_C(0x7FF))); }
  inline constexpr auto fracF64UI(std::uint64_t a) -> std::uint64_t { return static_cast<std::uint64_t>(a & static_cast<std::uint64_t>(UINT64_C(0x000FFFFFFFFFFFFF))); }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr auto packToF64UI(bool sign, IntegralTypeExp expA, IntegralTypeSig sig) -> std::uint64_t
  {
    return
      static_cast<std::uint64_t>
      (
          static_cast<std::uint64_t>(sign ? 1ULL << 63U : 0ULL)
        + static_cast<std::uint64_t>(static_cast<std::uint64_t>(expA) << 52U)
        + static_cast<std::uint64_t>(sig)
      );
  }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr auto packToF32UI(bool sign, IntegralTypeExp expA, IntegralTypeSig sig) -> std::uint32_t
  {
    return
      static_cast<std::uint32_t>
      (
          static_cast<std::uint32_t>(sign ? 1ULL << 31U : 0ULL)
        + static_cast<std::uint32_t>(static_cast<std::uint32_t>(expA) << 23U)
        + static_cast<std::uint32_t>(sig)
      );
  }

  inline constexpr auto softfloat_shortShiftRightJam64(std::uint64_t a, std::uint_fast16_t dist) -> std::uint64_t
  {
    return
      static_cast<std::uint64_t>
      (
          static_cast<std::uint64_t>(a >> dist)
        | static_cast<unsigned>
          (
            (static_cast<std::uint64_t>(a << static_cast<unsigned>(negate(dist) & 63U)) != static_cast<std::uint64_t>(UINT8_C(0)))
              ? static_cast<unsigned>(UINT8_C(1))
              : static_cast<unsigned>(UINT8_C(0))
          )
      );
  }

  inline constexpr auto softfloat_shiftRightJam64(std::uint64_t a, std::uint_fast16_t dist) -> std::uint64_t
  {
    return
      (dist < static_cast<std::uint32_t>(UINT8_C(63)))
        ? softfloat_shortShiftRightJam64(a, dist)
        : static_cast<std::uint64_t>
          (
            (a != static_cast<std::uint64_t>(UINT8_C(0)))
              ? static_cast<unsigned>(UINT8_C(1))
              : static_cast<unsigned>(UINT8_C(0))
          );
  }

  inline constexpr auto softfloat_countLeadingZeros8_z_table(std::uint_fast8_t index) -> std::uint_fast8_t
  {
    // A constant table that translates an 8-bit unsigned integer (the array index)
    // into the number of leading 0 bits before the most-significant 1 of that
    // integer.  For integer zero (index 0), the corresponding table element is 8.
    return   ((index < static_cast<std::uint_fast8_t>(UINT8_C(0x1))) ? static_cast<std::uint_fast8_t>(UINT8_C(4))
           : ((index < static_cast<std::uint_fast8_t>(UINT8_C(0x2))) ? static_cast<std::uint_fast8_t>(UINT8_C(3))
           : ((index < static_cast<std::uint_fast8_t>(UINT8_C(0x4))) ? static_cast<std::uint_fast8_t>(UINT8_C(2))
           : ((index < static_cast<std::uint_fast8_t>(UINT8_C(0x8))) ? static_cast<std::uint_fast8_t>(UINT8_C(1))
                                                                     : static_cast<std::uint_fast8_t>(UINT8_C(0))))));
  }

  inline constexpr auto softfloat_countLeadingZeros8(std::uint8_t a) -> std::uint_fast8_t
  {
    return
      (a < static_cast<std::uint_fast8_t>(UINT8_C(0x10)))
        ? static_cast<std::uint_fast8_t>(4U + softfloat_countLeadingZeros8_z_table(static_cast<std::uint_fast8_t>(a &  0xFU)))
        :                                     softfloat_countLeadingZeros8_z_table(static_cast<std::uint_fast8_t>(a >>   4U));
  }

  inline constexpr auto softfloat_countLeadingZeros16(std::uint16_t a) -> std::uint_fast8_t
  {
    return
      (a < static_cast<std::uint16_t>(UINT16_C(0x100)))
        ? static_cast<std::uint_fast8_t>(8U + softfloat_countLeadingZeros8(static_cast<std::uint8_t>(a)))
        :                                     softfloat_countLeadingZeros8(static_cast<std::uint8_t>(a >> 8U));
  }

  inline constexpr auto softfloat_countLeadingZeros32(std::uint32_t a) -> std::uint_fast8_t
  {
    return
      (a < static_cast<std::uint32_t>(UINT32_C(0x10000)))
        ? static_cast<std::uint_fast8_t>(16U + softfloat_countLeadingZeros16(static_cast<std::uint16_t>(a)))
        :                                      softfloat_countLeadingZeros16(static_cast<std::uint16_t>(a >> 16U));
  }

  inline constexpr auto softfloat_countLeadingZeros64(std::uint64_t a) -> std::uint_fast8_t
  {
    return
      (a < static_cast<std::uint64_t>(UINT64_C(0x100000000)))
        ? static_cast<std::uint_fast8_t>(32U + softfloat_countLeadingZeros32(static_cast<std::uint32_t>(a)))
        :                                      softfloat_countLeadingZeros32(static_cast<std::uint32_t>(a >> 32U));
  }

  inline constexpr auto softfloat_approxRecip32_1(std::uint32_t a) -> std::uint32_t
  {
    // Returns an approximation to the reciprocal of the number represented by a,
    // where a is interpreted as an unsigned fixed-point number with one integer
    // bit and 31 fraction bits.
    return static_cast<std::uint32_t>(static_cast<std::uint64_t>(UINT64_C(0x7FFFFFFFFFFFFFFF)) / a);
  }

  constexpr struct uint64_extra softfloat_shiftRightJam64Extra(std::uint64_t a, std::uint64_t extra, std::uint32_t dist)
  {
    // Shifts the 128 bits formed by concatenating a and extra right by 64
    // _plus_ the number of bits given in dist, which must not be zero. This
    // shifted value is at most 64 nonzero bits and is returned in the v field
    // of the struct uint64_extra result.
    return
    {
      (dist < 64U) ? static_cast<std::uint64_t>(a << static_cast<unsigned>(static_cast<unsigned>(negate(dist)) & 63U)) | (extra != 0U ? 1U : 0U) : ((dist == 64U) ? a : ((a != 0U) || (extra != 0U)) ? 1U : 0U),
      (dist < 64U) ? static_cast<std::uint64_t>(a >>                       static_cast<unsigned>       (dist))                                   : 0U
    };
  }

  template<const int BitCount,
           typename EnableType = void>
  struct uint_type_helper
  {
    static_assert(((BitCount >= 8) && (BitCount <= 64)),
                  "Error: uint_type_helper is not intended to be used for this BitCount");

    using exact_unsigned_type = std::uintmax_t;
  };

  template<const int BitCount> struct uint_type_helper<BitCount, typename std::enable_if<                    (BitCount <=  8)>::type> { using exact_unsigned_type = std::uint8_t;  };
  template<const int BitCount> struct uint_type_helper<BitCount, typename std::enable_if<(BitCount >=  9) && (BitCount <= 16)>::type> { using exact_unsigned_type = std::uint16_t; };
  template<const int BitCount> struct uint_type_helper<BitCount, typename std::enable_if<(BitCount >= 17) && (BitCount <= 32)>::type> { using exact_unsigned_type = std::uint32_t; };
  template<const int BitCount> struct uint_type_helper<BitCount, typename std::enable_if<(BitCount >= 33) && (BitCount <= 64)>::type> { using exact_unsigned_type = std::uint64_t; };

  template<typename BuiltInFloatType,
           typename ExactUnsignedIntegralType = typename uint_type_helper<std::numeric_limits<BuiltInFloatType>::digits>::exact_unsigned_type>
  union uz_type
  {
    static_assert((   std::is_same<BuiltInFloatType, float>::value
                   || std::is_same<BuiltInFloatType, double>::value),
                  "Error: This template is intended for either built-in float or double, but not for any other type(s)");

    using float_type    = BuiltInFloatType;
    using unsigned_type = ExactUnsignedIntegralType;

    const float_type    my_f;
    const unsigned_type my_u;

    constexpr uz_type(float_type    f) : my_f(f) { }
    constexpr uz_type(unsigned_type u) : my_u(u) { }
  };

  struct nothing { };

  } // namespace detail

  class soft_double;

  inline auto operator+(const soft_double& a, const soft_double& b) -> soft_double;
  inline auto operator-(const soft_double& a, const soft_double& b) -> soft_double;
  inline auto operator*(const soft_double& a, const soft_double& b) -> soft_double;
  inline auto operator/(const soft_double& a, const soft_double& b) -> soft_double;

  template<typename UnsignedIntegralType> auto operator+(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;
  template<typename UnsignedIntegralType> auto operator-(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;
  template<typename UnsignedIntegralType> auto operator*(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;
  template<typename UnsignedIntegralType> auto operator/(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;

  template<typename UnsignedIntegralType> auto operator+(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;
  template<typename UnsignedIntegralType> auto operator-(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;
  template<typename UnsignedIntegralType> auto operator*(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;
  template<typename UnsignedIntegralType> auto operator/(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type;

  template<typename SignedIntegralType> auto operator+(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;
  template<typename SignedIntegralType> auto operator-(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;
  template<typename SignedIntegralType> auto operator*(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;
  template<typename SignedIntegralType> auto operator/(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;

  template<typename SignedIntegralType> auto operator+(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;
  template<typename SignedIntegralType> auto operator-(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;
  template<typename SignedIntegralType> auto operator*(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;
  template<typename SignedIntegralType> auto operator/(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type;

  inline auto operator+(const soft_double& u, float f) -> soft_double;
  inline auto operator-(const soft_double& u, float f) -> soft_double;
  inline auto operator*(const soft_double& u, float f) -> soft_double;
  inline auto operator/(const soft_double& u, float f) -> soft_double;

  inline auto operator+(float f, const soft_double& u) -> soft_double;
  inline auto operator-(float f, const soft_double& u) -> soft_double;
  inline auto operator*(float f, const soft_double& u) -> soft_double;
  inline auto operator/(float f, const soft_double& u) -> soft_double;

  inline auto operator+(const soft_double& u, double f) -> soft_double;
  inline auto operator-(const soft_double& u, double f) -> soft_double;
  inline auto operator*(const soft_double& u, double f) -> soft_double;
  inline auto operator/(const soft_double& u, double f) -> soft_double;

  inline auto operator+(double f, const soft_double& u) -> soft_double;
  inline auto operator-(double f, const soft_double& u) -> soft_double;
  inline auto operator*(double f, const soft_double& u) -> soft_double;
  inline auto operator/(double f, const soft_double& u) -> soft_double;

  inline auto operator+(const soft_double& u, long double f) -> soft_double;
  inline auto operator-(const soft_double& u, long double f) -> soft_double;
  inline auto operator*(const soft_double& u, long double f) -> soft_double;
  inline auto operator/(const soft_double& u, long double f) -> soft_double;

  inline auto operator+(long double f, const soft_double& u) -> soft_double;
  inline auto operator-(long double f, const soft_double& u) -> soft_double;
  inline auto operator*(long double f, const soft_double& u) -> soft_double;
  inline auto operator/(long double f, const soft_double& u) -> soft_double;

  inline constexpr auto operator< (const soft_double& a, const soft_double& b) -> bool;
  inline constexpr auto operator<=(const soft_double& a, const soft_double& b) -> bool;
  inline constexpr auto operator==(const soft_double& a, const soft_double& b) -> bool;
  inline constexpr auto operator!=(const soft_double& a, const soft_double& b) -> bool;
  inline constexpr auto operator>=(const soft_double& a, const soft_double& b) -> bool;
  inline constexpr auto operator> (const soft_double& a, const soft_double& b) -> bool;

  template<typename UnsignedIntegralType> auto operator< (const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator<=(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator==(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator!=(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator>=(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator> (const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;

  template<typename UnsignedIntegralType> auto operator< (const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator<=(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator==(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator!=(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator>=(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;
  template<typename UnsignedIntegralType> auto operator> (const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type;

  template<typename SignedIntegralType> auto operator< (const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator<=(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator==(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator!=(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator>=(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator> (const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;

  template<typename SignedIntegralType> auto operator< (const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator<=(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator==(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator!=(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator>=(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;
  template<typename SignedIntegralType> auto operator> (const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type;

  inline auto operator< (const soft_double& a, const float f) -> bool;
  inline auto operator<=(const soft_double& a, const float f) -> bool;
  inline auto operator==(const soft_double& a, const float f) -> bool;
  inline auto operator!=(const soft_double& a, const float f) -> bool;
  inline auto operator>=(const soft_double& a, const float f) -> bool;
  inline auto operator> (const soft_double& a, const float f) -> bool;

  inline auto operator< (const float f, const soft_double& a) -> bool;
  inline auto operator<=(const float f, const soft_double& a) -> bool;
  inline auto operator==(const float f, const soft_double& a) -> bool;
  inline auto operator!=(const float f, const soft_double& a) -> bool;
  inline auto operator>=(const float f, const soft_double& a) -> bool;
  inline auto operator> (const float f, const soft_double& a) -> bool;

  inline auto operator< (const soft_double& a, const double f) -> bool;
  inline auto operator<=(const soft_double& a, const double f) -> bool;
  inline auto operator==(const soft_double& a, const double f) -> bool;
  inline auto operator!=(const soft_double& a, const double f) -> bool;
  inline auto operator>=(const soft_double& a, const double f) -> bool;
  inline auto operator> (const soft_double& a, const double f) -> bool;

  inline auto operator< (const double f, const soft_double& a) -> bool;
  inline auto operator<=(const double f, const soft_double& a) -> bool;
  inline auto operator==(const double f, const soft_double& a) -> bool;
  inline auto operator!=(const double f, const soft_double& a) -> bool;
  inline auto operator>=(const double f, const soft_double& a) -> bool;
  inline auto operator> (const double f, const soft_double& a) -> bool;

  inline auto operator< (const soft_double& a, const long double f) -> bool;
  inline auto operator<=(const soft_double& a, const long double f) -> bool;
  inline auto operator==(const soft_double& a, const long double f) -> bool;
  inline auto operator!=(const soft_double& a, const long double f) -> bool;
  inline auto operator>=(const soft_double& a, const long double f) -> bool;
  inline auto operator> (const soft_double& a, const long double f) -> bool;

  inline auto operator< (const long double f, const soft_double& a) -> bool;
  inline auto operator<=(const long double f, const soft_double& a) -> bool;
  inline auto operator==(const long double f, const soft_double& a) -> bool;
  inline auto operator!=(const long double f, const soft_double& a) -> bool;
  inline auto operator>=(const long double f, const soft_double& a) -> bool;
  inline auto operator> (const long double f, const soft_double& a) -> bool;

  #if !defined(SOFT_DOUBLE_DISABLE_IOSTREAM)
  template<typename char_type, typename traits_type> auto operator<<(std::basic_ostream<char_type, traits_type>& os, const soft_double& f) -> std::basic_ostream<char_type, traits_type>&;
  template<typename char_type, typename traits_type> auto operator>>(std::basic_istream<char_type, traits_type>& is,       soft_double& f) -> std::basic_istream<char_type, traits_type>&;
  #endif // !WIDE_DECIMAL_DISABLE_IOSTREAM

  inline auto isnan   (soft_double x) -> bool;
  inline auto isinf   (soft_double x) -> bool;
  inline auto isfinite(soft_double x) -> bool;
  inline auto fabs    (soft_double x) -> soft_double;
  inline auto frexp   (soft_double x, int* expptr) -> soft_double;
  inline auto ldexp   (soft_double x, int expval) -> soft_double;
  inline auto floor   (soft_double x) -> soft_double;
  inline auto ceil    (soft_double x) -> soft_double;
  inline auto sqrt    (soft_double x) -> soft_double;
  inline auto exp     (soft_double x) -> soft_double;
  inline auto log     (soft_double x) -> soft_double;
  inline auto pow     (soft_double x, soft_double a) -> soft_double;
  inline auto sin     (soft_double x) -> soft_double;
  inline auto cos     (soft_double x) -> soft_double;
  inline auto tan     (soft_double x) -> soft_double;
  inline auto sinh    (soft_double x) -> soft_double;
  inline auto cosh    (soft_double x) -> soft_double;
  inline auto tanh    (soft_double x) -> soft_double;

  template<typename UnsignedIntegralType,
           typename std::enable_if<(   (std::is_integral<UnsignedIntegralType>::value == true)
                                    && (std::is_unsigned<UnsignedIntegralType>::value == true))>::type const* = nullptr>
  auto pow(const soft_double x, const UnsignedIntegralType u) -> soft_double;

  template<typename SignedIntegralType,
           typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                    && (std::is_signed  <SignedIntegralType>::value == true))>::type const* = nullptr>
  auto pow(const soft_double x, const SignedIntegralType n) -> soft_double;

  class soft_double final
  {
    static_assert(sizeof(float) == 4U,
                  "Error: This template is designed for 4 byte built-in float");
  public:
    using representation_type = std::uint64_t;

    soft_double() = default;

    template<typename UnsignedIntegralType,
             typename std::enable_if<(   (std::is_integral<UnsignedIntegralType>::value == true)
                                      && (std::is_unsigned<UnsignedIntegralType>::value == true)
                                      && (sizeof(UnsignedIntegralType) <= sizeof(std::uint32_t)))>::type const* = nullptr>
    constexpr soft_double(UnsignedIntegralType u) : my_value(my_ui32_to_f64((std::uint32_t) u)) { }

    template<typename UnsignedIntegralType,
             typename std::enable_if<(   (std::is_integral<UnsignedIntegralType>::value == true)
                                      && (std::is_unsigned<UnsignedIntegralType>::value == true)
                                      && !(sizeof(UnsignedIntegralType) <= sizeof(std::uint32_t)))>::type const* = nullptr>
    constexpr soft_double(UnsignedIntegralType u) : my_value(my_ui64_to_f64((std::uint64_t) u)) { }

    template<typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed  <SignedIntegralType>::value == true)
                                      && (sizeof(SignedIntegralType) <= sizeof(int32_t)))>::type const* = nullptr>
    constexpr soft_double(SignedIntegralType n) : my_value(my__i32_to_f64((std::int32_t) n)) { }

    template<typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed  <SignedIntegralType>::value == true)
                                      && !(sizeof(SignedIntegralType) <= sizeof(int32_t)))>::type const* = nullptr>
    constexpr soft_double(SignedIntegralType n) : my_value(my__i64_to_f64((std::int64_t) n)) { }

    constexpr soft_double(float f)
      : my_value
        (
          ((detail::expF32UI (detail::uz_type<float>(f).my_u) == 0) && (detail::fracF32UI(detail::uz_type<float>(f).my_u) == 0U))
            ? detail::packToF64UI(detail::signF32UI(detail::uz_type<float>(f).my_u), 0, 0)
            : detail::packToF64UI(detail::signF32UI(detail::uz_type<float>(f).my_u), detail::expF32UI(detail::uz_type<float>(f).my_u) + 0x380, (std::uint64_t) detail::fracF32UI(detail::uz_type<float>(f).my_u) << 29)
        ) { }

    constexpr soft_double(double d)
      : my_value(detail::uz_type<double>(d).my_u) { }

    constexpr soft_double(const long double ld)
      : my_value(detail::uz_type<double>(static_cast<double>(ld)).my_u) { }

    constexpr soft_double(const soft_double&) = default;

    constexpr soft_double(soft_double&& other) noexcept
      : my_value(other.my_value) { }

    constexpr soft_double(std::uint64_t n, detail::nothing&&) noexcept
      : my_value(static_cast<std::uint64_t>(n)) { }

    ~soft_double() = default;

    auto operator=(const soft_double&) -> soft_double& = default;

    auto operator=(soft_double&&) noexcept -> soft_double& = default;

    auto  representation()       ->       representation_type& { return my_value; }
    auto  representation() const -> const representation_type& { return my_value; }
    auto crepresentation() const -> const representation_type& { return my_value; }

    static constexpr auto get_rep(soft_double a) -> representation_type { return a.my_value; }

    operator   signed char     () const { return   static_cast<signed char>     (f64_to__i32(my_value)); }
    operator   signed short    () const { return   static_cast<signed short>    (f64_to__i32(my_value)); }
    operator   signed int      () const { return   static_cast<signed int>      (f64_to__i32(my_value)); }
    operator   signed long     () const { return   static_cast<signed long>     (f64_to__i64(my_value)); }
    operator   signed long long() const { return   static_cast<signed long long>(f64_to__i64(my_value)); }

    operator unsigned char     () const { return static_cast<unsigned char>     (f64_to_ui32(my_value)); }
    operator unsigned short    () const { return static_cast<unsigned short>    (f64_to_ui32(my_value)); }
    operator unsigned int      () const { return static_cast<unsigned int>      (f64_to_ui32(my_value)); }
    operator unsigned long     () const { return static_cast<unsigned long>     (f64_to_ui64(my_value)); }
    operator unsigned long long() const { return static_cast<unsigned long long>(f64_to_ui64(my_value)); }

    operator float      () const noexcept { return to_float<float>(); }
    operator double     () const noexcept { return to_float<double>(); }
    operator long double() const noexcept { return static_cast<long double>((double) *this); }

    auto operator+=(const soft_double& other) -> soft_double& { my_value = f64_add(my_value, other.my_value); return *this; }
    auto operator-=(const soft_double& other) -> soft_double& { my_value = f64_sub(my_value, other.my_value); return *this; }
    auto operator*=(const soft_double& other) -> soft_double& { my_value = f64_mul(my_value, other.my_value); return *this; }
    auto operator/=(const soft_double& other) -> soft_double& { my_value = f64_div(my_value, other.my_value); return *this; }

    // Operators pre-increment and pre-decrement.
    auto operator++() -> soft_double& { return *this += my_value_one(); }
    auto operator--() -> soft_double& { return *this -= my_value_one(); }

    // Operators post-increment and post-decrement.
    auto operator++(int) -> soft_double { const soft_double w(*this); static_cast<void>(++(*this)); return w; }
    auto operator--(int) -> soft_double { const soft_double w(*this); static_cast<void>(--(*this)); return w; }

    auto operator+() const -> const soft_double& { return *this; }
    auto operator-() const ->       soft_double  { return soft_double(my_value ^ (std::uint64_t) (1ULL << 63U), detail::nothing()); }

    static constexpr auto my_value_zero   () -> soft_double { return soft_double(UINT64_C(0),                   detail::nothing()); }
    static constexpr auto my_value_one    () -> soft_double { return soft_double(UINT64_C(0x3FF0000000000000),  detail::nothing()); }
    static constexpr auto my_value_two    () -> soft_double { return soft_double(UINT64_C(0x4000000000000000),  detail::nothing()); }
    static constexpr auto my_value_half   () -> soft_double { return soft_double(UINT64_C(0x3FE0000000000000),  detail::nothing()); }
    static constexpr auto my_value_pi     () -> soft_double { return soft_double(UINT64_C(4614256656552045848), detail::nothing()); }
    static constexpr auto my_value_pi_half() -> soft_double { return soft_double(UINT64_C(0x3FF921FB54442D18),  detail::nothing()); }
    static constexpr auto my_value_ln2    () -> soft_double { return soft_double(UINT64_C(4604418534313441775), detail::nothing()); }

    static constexpr auto my_value_min()           -> soft_double { return soft_double(UINT64_C(4503599627370496),    detail::nothing()); }
    static constexpr auto my_value_max()           -> soft_double { return soft_double(UINT64_C(9218868437227405311), detail::nothing()); }
    static constexpr auto my_value_lowest()        -> soft_double { return soft_double(UINT64_C(18442240474082181119),detail::nothing()); }
    static constexpr auto my_value_epsilon()       -> soft_double { return soft_double(UINT64_C(4372995238176751616), detail::nothing()); }
    static constexpr auto my_value_round_error()   -> soft_double { return soft_double(UINT64_C(0x3FE0000000000000),  detail::nothing()); }
    static constexpr auto my_value_denorm_min()    -> soft_double { return soft_double(UINT64_C(1),                   detail::nothing()); }
    static constexpr auto my_value_infinity()      -> soft_double { return soft_double(UINT64_C(0x7FF0000000000000),  detail::nothing()); }
    static constexpr auto my_value_quiet_NaN()     -> soft_double { return soft_double(UINT64_C(0xFFF8000000000000),  detail::nothing()); }
    static constexpr auto my_value_signaling_NaN() -> soft_double { return soft_double(UINT64_C(0x7FF8000000000000),  detail::nothing()); }

  private:
    representation_type my_value;

    template<typename FloatingPointType>
    auto to_float() const -> typename std::enable_if<(   (sizeof(FloatingPointType) == 4)
                                                      && (std::numeric_limits<FloatingPointType>::is_iec559 == true)), FloatingPointType>::type
    {
      return f64_to_f32(my_value);
    }

    template<typename FloatingPointType>
    auto to_float() const -> typename std::enable_if<(   (sizeof(FloatingPointType) == 8)
                                                      && (std::numeric_limits<FloatingPointType>::is_iec559 == true)), FloatingPointType>::type
    {
      return (FloatingPointType) (*(volatile FloatingPointType*) (this));
    }

    static constexpr auto my_le(const soft_double& a, const soft_double& b) -> bool
    {
      return (detail::signF64UI(a.my_value) != detail::signF64UI(b.my_value))
               ? (detail::signF64UI(a.my_value) || !((a.my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF)))
               : (a.my_value == b.my_value) || (detail::signF64UI(a.my_value) ^ (a.my_value < b.my_value));
    }

    static constexpr auto my_lt(const soft_double& a, const soft_double& b) -> bool
    {
      return (detail::signF64UI(a.my_value) != detail::signF64UI(b.my_value))
               ? detail::signF64UI(a.my_value) && ((a.my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF))
               : (a.my_value != b.my_value) && (detail::signF64UI(a.my_value) ^ (a.my_value < b.my_value));
    }

    static auto f64_add(const std::uint64_t a, const std::uint64_t b) -> std::uint64_t
    {
      const auto signA = detail::signF64UI(a);

      return ((signA == detail::signF64UI(b)) ? softfloat_addMagsF64(a, b, signA)
                                              : softfloat_subMagsF64(a, b, signA));
    }

    static auto f64_sub(const std::uint64_t a, const std::uint64_t b) -> std::uint64_t
    {
      const auto signA = detail::signF64UI(a);

      return ((signA == detail::signF64UI(b)) ? softfloat_subMagsF64(a, b, signA)
                                              : softfloat_addMagsF64(a, b, signA));
    }

    static auto f64_mul(const std::uint64_t a, const std::uint64_t b) -> std::uint64_t
    {
      const auto signA = detail::signF64UI(a);
      const auto expA  = detail::expF64UI (a);
            auto sigA  = detail::fracF64UI(a);

      const auto signB = detail::signF64UI(b);
      const auto expB  = detail::expF64UI (b);
            auto sigB  = detail::fracF64UI(b);

      const bool signZ = (signA ^ signB);

      auto result = std::uint64_t { };

      if(    (   (expA == static_cast<std::int16_t> ( INT8_C(0)))
              && (sigA == static_cast<std::uint64_t>(UINT8_C(0))))
          || (   (expB == static_cast<std::int16_t> ( INT8_C(0)))
              && (sigB == static_cast<std::uint64_t>(UINT8_C(0)))))
      {
        result = detail::packToF64UI(signZ, 0, 0);
      }
      else
      {
        std::int16_t expZ =
          static_cast<std::int16_t>
          (
              static_cast<std::int16_t>(expA + expB)
            - static_cast<std::int16_t>(INT16_C(0x3FF))
          );

        sigA = static_cast<std::uint64_t>(static_cast<std::uint64_t>(sigA | static_cast<std::uint64_t>(UINT64_C(0x0010000000000000))) << 10U);
        sigB = static_cast<std::uint64_t>(static_cast<std::uint64_t>(sigB | static_cast<std::uint64_t>(UINT64_C(0x0010000000000000))) << 11U);

        // Compute the 128-bit product of sigA and sigB.

        const auto a32 = static_cast<std::uint32_t>(sigA >> 32U);
        const auto a0  = static_cast<std::uint32_t>(sigA);
        const auto b32 = static_cast<std::uint32_t>(sigB >> 32U);
        const auto b0  = static_cast<std::uint32_t>(sigB);

        struct detail::uint128_as_struct sig128Z;

        sig128Z.v0 = static_cast<std::uint64_t>(static_cast<std::uint64_t>(a0) * b0);

        auto mid1 = static_cast<std::uint64_t>(                                  (static_cast<std::uint64_t>(a32)) * b0);
        auto mid  = static_cast<std::uint64_t>(mid1 + static_cast<std::uint64_t>((static_cast<std::uint64_t>(b32)) * a0));

        sig128Z.v64  = static_cast<std::uint64_t> (static_cast<std::uint64_t>(a32) * b32);
        sig128Z.v64 += static_cast<std::uint64_t>((static_cast<std::uint64_t>(mid < mid1) << 32U) | static_cast<std::uint32_t>(mid >> 32U));

        mid <<= 32U;

        sig128Z.v0  += mid;
        sig128Z.v64 += (sig128Z.v0 < mid);

        if(sig128Z.v0 != 0U)
        {
          sig128Z.v64 |= 1U;
        }

        if(sig128Z.v64 < static_cast<std::uint64_t>(UINT64_C(0x4000000000000000)))
        {
          --expZ;

          sig128Z.v64 <<= 1U;
        }

        result = softfloat_roundPackToF64(signZ, expZ, sig128Z.v64);
      }

      return result;
    }

    static auto f64_div(const std::uint64_t a, const std::uint64_t b) -> std::uint64_t
    {
      const auto signA = detail::signF64UI(a);
            auto expA  = detail::expF64UI (a);
            auto sigA  = detail::fracF64UI(a);

      const auto signB = detail::signF64UI(b);
            auto expB  = detail::expF64UI (b);
            auto sigB  = detail::fracF64UI(b);

      const bool signZ = signA ^ signB;

      auto result = std::uint64_t { };

      const auto a_is_zero = (   (expA == static_cast<std::int16_t> ( INT8_C(0)))
                              && (sigA == static_cast<std::uint64_t>(UINT8_C(0))));

      if(a_is_zero)
      {
        result = detail::packToF64UI(signZ, 0, 0);
      }
      else
      {
        auto expZ =
          static_cast<std::int16_t>
          (
            static_cast<std::int16_t>(expA - expB) + static_cast<std::int16_t>(INT16_C(0x3FE))
          );

        sigA |= static_cast<std::uint64_t>(UINT64_C(0x0010000000000000));
        sigB |= static_cast<std::uint64_t>(UINT64_C(0x0010000000000000));

        if(sigA < sigB)
        {
          --expZ;

          sigA <<= 11U;
        }
        else
        {
          sigA <<= 10U;
        }

        sigB <<= 11U;

        const auto recip32 = static_cast<std::uint32_t>(detail::softfloat_approxRecip32_1(static_cast<std::uint32_t>(sigB >> 32U)) - 2U);
        const auto sig32Z  = static_cast<std::uint32_t>(static_cast<std::uint64_t>(static_cast<std::uint32_t>(sigA >> 32U) * static_cast<std::uint64_t>(recip32)) >> 32U);

        auto doubleTerm = static_cast<std::uint32_t>(sig32Z << 1U);

        auto rem =
          static_cast<std::uint64_t>
          (
            static_cast<std::uint64_t>
            (
              (sigA - static_cast<std::uint64_t>(static_cast<std::uint64_t>(doubleTerm) * static_cast<std::uint32_t>(sigB >> 32U))) << 28U
            )
          - static_cast<std::uint64_t>
            (
              static_cast<std::uint64_t>(doubleTerm) * static_cast<std::uint32_t>(static_cast<std::uint32_t>(sigB) >> 4U)
            )
          );

        auto q =
          static_cast<std::uint32_t>
          (
              static_cast<std::uint32_t>
              (
                   static_cast<std::uint64_t>(static_cast<std::uint32_t>(rem >> 32U) * static_cast<std::uint64_t>(recip32))
                >> 32U
              )
            + 4U
          );

        auto sigZ =
          static_cast<std::uint64_t>
          (
              static_cast<std::uint64_t>(static_cast<std::uint64_t>(sig32Z) << 32U)
            + static_cast<std::uint64_t>(static_cast<std::uint64_t>(q)      <<  4U)
          );

        if(static_cast<std::uint_fast16_t>(sigZ & 0x1FFU) < static_cast<std::uint_fast16_t>(4UL << 4U))
        {
          q    &= static_cast<std::uint32_t>(~static_cast<std::uint32_t>(UINT8_C(7)));
          sigZ &= static_cast<std::uint64_t>(~static_cast<std::uint64_t>(UINT8_C(0x7F)));

          doubleTerm = static_cast<std::uint32_t>(q << 1U);

          rem =
            static_cast<std::uint64_t>
            (
                static_cast<std::uint64_t>
                (
                  static_cast<std::uint64_t>
                  (
                    rem - static_cast<std::uint64_t>(static_cast<std::uint64_t>(doubleTerm) * static_cast<std::uint32_t>(sigB >> 32U))
                  ) << 28U
                )
              - static_cast<std::uint64_t>
                (
                  static_cast<std::uint64_t>(doubleTerm) * static_cast<std::uint32_t>(static_cast<std::uint32_t>(sigB) >> 4U)
                )
            );

          const auto rem_hi_bit_is_set = (static_cast<unsigned>(rem >> 63U) != static_cast<unsigned>(UINT8_C(0)));

          if(rem_hi_bit_is_set)
          {
            sigZ = static_cast<std::uint64_t>(sigZ - static_cast<std::uint_fast8_t>(1UL << 7U));
          }
          else
          {
            if(rem != static_cast<std::uint64_t>(UINT8_C(0)))
            {
              sigZ = static_cast<std::uint64_t>(sigZ | static_cast<std::uint_fast8_t>(UINT8_C(1)));
            }
          }
        }

        result = softfloat_roundPackToF64(signZ, expZ, sigZ);
      }

      return result;
    }

    static auto f64_sqrt(const std::uint64_t a) -> std::uint64_t
    {
      auto expA = detail::expF64UI (a);
      auto sigA = detail::fracF64UI(a);

      auto result = std::uint64_t { };

      if(  (   (expA == static_cast<std::int16_t> ( INT8_C(0)))
            && (sigA == static_cast<std::uint64_t>(UINT8_C(0))))
         || detail::signF64UI(a))
      {
        result = a;
      }
      else
      {
        // sig32Z is guaranteed to be a lower bound on the square root
        // of sig32A, which makes sig32Z also a lower bound on the
        // square root of sigA.

        const auto expZ =
          static_cast<std::int16_t>
          (
              static_cast<std::int16_t>(static_cast<std::int16_t>(expA - static_cast<std::int16_t>(INT16_C(0x3FF))) >> 1U)
            + static_cast<std::int16_t>(INT16_C(0x3FE))
          );

        expA = static_cast<std::int16_t>(expA & static_cast<std::int16_t>(INT8_C(1)));
        sigA = static_cast<std::uint64_t>(sigA | static_cast<std::uint64_t>(UINT64_C(0x0010000000000000)));

        const auto sig32A      = static_cast<std::uint32_t>(sigA >> 21U);
              auto recipSqrt32 = softfloat_approxRecipSqrt32_1(static_cast<std::uint32_t>(expA), sig32A);
              auto sig32Z      = static_cast<std::uint32_t>
                                 (
                                   static_cast<std::uint64_t>(static_cast<std::uint64_t>(sig32A) * recipSqrt32) >> 32U
                                 );

        if(expA != static_cast<std::int16_t>(INT8_C(0)))
        {
          sigA   <<= 8U;
          sig32Z >>= 1U;
        }
        else
        {
          sigA <<= 9U;
        }

              auto rem = static_cast<std::uint64_t>(sigA - static_cast<std::uint64_t>(static_cast<std::uint64_t>(sig32Z) * sig32Z));
        const auto q   = static_cast<std::uint32_t>
                         (
                           static_cast<std::uint64_t>
                           (
                             static_cast<std::uint32_t>(rem >> 2U) * static_cast<std::uint64_t>(recipSqrt32)
                           ) >> 32U
                         );

        auto sigZ =
          static_cast<std::uint64_t>
          (
              static_cast<std::uint64_t>
              (
                  static_cast<std::uint64_t>(static_cast<std::uint64_t>(sig32Z) << 32U)
                | static_cast<std::uint_fast8_t>(1UL << 5U)
              )
            + static_cast<std::uint64_t>((static_cast<std::uint64_t>(q) << 3U))
          );

        const auto sig_z_is_small =
          (static_cast<std::uint_fast16_t>(sigZ & static_cast<std::uint_fast16_t>(UINT16_C(0x1FF))) < static_cast<std::uint_fast16_t>(UINT16_C(0x22)));

        if(sig_z_is_small)
        {
          sigZ &= static_cast<std::uint64_t>(~static_cast<std::uint64_t>(UINT8_C(0x3F)));

          const auto shiftedSigZ = static_cast<std::uint64_t>(sigZ >> 6U);

          rem =
            static_cast<std::uint64_t>
            (
                static_cast<std::uint64_t>(sigA << 52U)
              - static_cast<std::uint64_t>(shiftedSigZ * shiftedSigZ)
            );

          const auto rem_hi_bit_is_set = (static_cast<unsigned>(rem >> 63U) != static_cast<unsigned>(UINT8_C(0)));

          if(rem_hi_bit_is_set)
          {
            --sigZ;
          }
          else
          {
            if(rem != static_cast<std::uint64_t>(UINT8_C(0)))
            {
              sigZ |= 1U;
            }
          }
        }

        result = softfloat_roundPackToF64(0, expZ, sigZ);
      }

      return result;
    }

    static auto f64_to_ui32(const std::uint64_t a) -> std::uint32_t
    {
      const auto expA = detail::expF64UI (a);
            auto sig  = detail::fracF64UI(a);

      if(expA != static_cast<std::int16_t>(INT8_C(0)))
      {
        sig |= static_cast<std::uint64_t>(UINT64_C(0x0010000000000000));
      }

      const auto shiftDist = static_cast<std::int16_t>(static_cast<std::int16_t>(INT16_C(0x427)) - expA);

      if(static_cast<std::int16_t>(INT16_C(0)) < shiftDist)
      {
        sig = detail::softfloat_shiftRightJam64(sig, static_cast<std::uint_fast16_t>(shiftDist));
      }

      return softfloat_roundToUI32(detail::signF64UI(a), sig);
    }

    static int32_t f64_to__i32(const std::uint64_t a)
    {
      const auto expA = detail::expF64UI (a);
            auto sig  = detail::fracF64UI(a);

      if(expA != static_cast<std::int16_t>(INT8_C(0)))
      {
        sig |= static_cast<std::uint64_t>(UINT64_C(0x0010000000000000));
      }

      const auto shiftDist = static_cast<std::int16_t>(static_cast<std::int16_t>(INT16_C(0x427)) - expA);

      if(static_cast<std::int16_t>(INT16_C(0)) < shiftDist)
      {
        sig = detail::softfloat_shiftRightJam64(sig, static_cast<std::uint_fast16_t>(shiftDist));
      }

      return softfloat_roundToI32(detail::signF64UI(a), sig);
    }

    static std::uint64_t f64_to_ui64(const std::uint64_t a)
    {
      const auto expA = detail::expF64UI (a);
            auto sig  = detail::fracF64UI(a);

      if(expA)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      std::int16_t shiftDist = (std::int16_t) ((std::int16_t) 0x433 - expA);

      struct detail::uint64_extra sigExtra;

      if(shiftDist <= 0)
      {
        if(shiftDist < -11)
        {
          shiftDist = -11;
        }

        sigExtra.v = sig << -shiftDist;
        sigExtra.extra = 0;
      }
      else
      {
        sigExtra = detail::softfloat_shiftRightJam64Extra(sig, 0U, (std::uint32_t) shiftDist);
      }

      return softfloat_roundToUI64(detail::signF64UI(a), sigExtra.v);
    }

    static auto f64_to__i64(const std::uint64_t a) -> std::int64_t
    {
      const auto sign = detail::signF64UI(a);
      const auto expA = detail::expF64UI (a);
            auto sig  = detail::fracF64UI(a);

      if(expA)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      std::int16_t shiftDist = (std::int16_t) (INT16_C(0x433) - expA);

      struct detail::uint64_extra sigExtra;

      if(shiftDist <= 0)
      {
        if(shiftDist < -11)
        {
          shiftDist = -11;
        }

        sigExtra.v = sig << -shiftDist;
        sigExtra.extra = 0;
      }
      else
      {
        sigExtra = detail::softfloat_shiftRightJam64Extra(sig, 0U, (std::uint32_t) shiftDist);
      }

      return softfloat_roundToI64(sign, sigExtra.v);
    }

    static auto f64_to_f32(const std::uint64_t a) -> float
    {
      const auto frac32 =
        static_cast<std::uint32_t>
        (
          detail::softfloat_shortShiftRightJam64(detail::fracF64UI(a), 22)
        );

      return
        softfloat_roundPackToF32
        (
          detail::signF64UI(a),
          static_cast<std::int16_t>(detail::expF64UI(a) - static_cast<std::int16_t>(INT16_C(0x381))),
          static_cast<std::uint32_t>(frac32 | static_cast<std::uint32_t>(UINT32_C(0x40000000)))
        );
    }

    static auto softfloat_roundPackToF32(bool sign, std::int16_t expA, std::uint32_t sig) -> float
    {
      constexpr auto roundIncrement = static_cast<std::uint_fast8_t>(UINT8_C(0x40));

      const uint_fast8_t roundBits = sig & 0x7F;

      sig = static_cast<std::uint32_t>((sig + roundIncrement) >> 7U);

      sig =
        static_cast<std::uint32_t>
        (
            sig
          & static_cast<std::uint32_t>(~static_cast<std::uint32_t>(((roundBits ^ 0x40) == 0U ? 1U : 0U) & 1U))
        );

      if(!sig)
      {
        expA = 0;
      }

      return detail::uz_type<float>(detail::packToF32UI(sign, expA, sig)).my_f;
    }

    static constexpr auto my__i32_to_f64(const int32_t a) -> std::uint64_t
    {
      return
        (!a) ? 0U
             : detail::packToF64UI((a < 0), 0x432 - int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(std::uint32_t((a < 0) ? detail::negate((std::uint32_t) a) : (std::uint32_t) a)) + 21U)), (std::uint64_t) std::uint32_t((a < 0) ? detail::negate((std::uint32_t) a) : (std::uint32_t) a) << int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(std::uint32_t((a < 0) ? detail::negate((std::uint32_t) a) : (std::uint32_t) a)) + 21U)));
    }

    static constexpr auto my__i64_to_f64(const int64_t a) -> std::uint64_t
    {
      return
        (!((std::uint64_t) a & UINT64_C(0x7FFFFFFFFFFFFFFF)))
          ? ((a < 0) ? detail::packToF64UI(1, 0x43E, 0) : 0U)
          : softfloat_normRoundPackToF64((a < 0), 0x43C, std::uint64_t((a < 0) ? detail::negate((std::uint64_t) a) : (std::uint64_t) a));
    }

    static constexpr auto my_ui32_to_f64(const std::uint32_t a) -> std::uint64_t
    {
      return
        ((a == 0U)
          ? 0U
          : detail::packToF64UI(0, 0x432 - int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(a) + 21U)), ((std::uint64_t) a) << int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(a) + 21U))));
    }

    static constexpr auto my_ui64_to_f64(const std::uint64_t a) -> std::uint64_t
    {
      return (!a) ? (std::uint64_t) 0U
                  : (a & UINT64_C(0x8000000000000000)) ? softfloat_roundPackToF64(0, 0x43D, detail::softfloat_shortShiftRightJam64(a, 1))
                                                       : softfloat_normRoundPackToF64(0, 0x43C, a);
    }

    static constexpr auto softfloat_roundToI32(bool sign, std::uint64_t sig) -> std::int32_t
    {
      return (int32_t) ((std::uint32_t) (sign ? detail::negate((std::uint32_t) (sig >> 12U)) : (std::uint32_t) (sig >> 12U)));
    }

    static constexpr auto softfloat_roundToI64(bool sign, std::uint64_t sig) -> std::int64_t
    {
      return (int64_t) ((std::uint64_t) (sign ? detail::negate(sig) : sig));
    }

    static constexpr auto softfloat_roundToUI32(bool sign, std::uint64_t sig) -> std::uint32_t
    {
      return ((sign) && (sig == 0U)) ? 0U : (std::uint32_t) (sig >> 12);
    }

    static constexpr auto softfloat_roundToUI64(bool sign, std::uint64_t sig) -> std::uint64_t
    {
      return ((sign) && (sig == 0U)) ? 0U : sig;
    }

    static auto softfloat_addMagsF64(std::uint64_t uiA, std::uint64_t uiB, bool signZ) -> std::uint64_t
    {
      const auto expA = detail::expF64UI (uiA);
            auto sigA = detail::fracF64UI(uiA);
      const auto expB = detail::expF64UI (uiB);
            auto sigB = detail::fracF64UI(uiB);

      const auto expDiff = static_cast<std::int16_t>(expA - expB);

      std::int16_t  expZ;
      std::uint64_t sigZ;

      if(expDiff == 0)
      {
        expZ = expA;
        sigZ = (std::uint64_t) (UINT64_C(0x0020000000000000) + sigA) + sigB;
        sigZ <<= 9U;
      }
      else
      {
        sigA <<= 9U;
        sigB <<= 9U;

        if(expDiff < 0)
        {
          expZ = expB;

          if(expA)
          {
            sigA += UINT64_C(0x2000000000000000);
          }
          else
          {
            sigA <<= 1;
          }

          sigA = detail::softfloat_shiftRightJam64(sigA, static_cast<std::uint_fast16_t>(-expDiff));
        }
        else
        {
          expZ = expA;

          if(expB)
          {
            sigB += UINT64_C(0x2000000000000000);
          }
          else
          {
            sigB <<= 1U;
          }

          sigB = detail::softfloat_shiftRightJam64(sigB, static_cast<std::uint_fast16_t>(expDiff));
        }

        sigZ = (std::uint64_t) (UINT64_C(0x2000000000000000) + sigA) + sigB;

        if(sigZ < UINT64_C(0x4000000000000000))
        {
          --expZ;

          sigZ <<= 1U;
        }
      }

      return softfloat_roundPackToF64(signZ, expZ, sigZ);
    }

    static auto softfloat_approxRecipSqrt32_1(std::uint32_t oddExpA, std::uint32_t a) -> std::uint32_t
    {
      // Returns an approximation to the reciprocal of the square root of the number
      // represented by a, where a is interpreted as an unsigned fixed-point
      // number either with one integer bit and 31 fraction bits or with two integer
      // bits and 30 fraction bits.
      constexpr std::array<std::uint16_t, 16U> softfloat_approxRecipSqrt_1k0s =
      {{
        UINT16_C(0xB4C9), UINT16_C(0xFFAB), UINT16_C(0xAA7D), UINT16_C(0xF11C),
        UINT16_C(0xA1C5), UINT16_C(0xE4C7), UINT16_C(0x9A43), UINT16_C(0xDA29),
        UINT16_C(0x93B5), UINT16_C(0xD0E5), UINT16_C(0x8DED), UINT16_C(0xC8B7),
        UINT16_C(0x88C6), UINT16_C(0xC16D), UINT16_C(0x8424), UINT16_C(0xBAE1)
      }};

      constexpr std::array<std::uint16_t, 16U> softfloat_approxRecipSqrt_1k1s =
      {{
        UINT16_C(0xA5A5), UINT16_C(0xEA42), UINT16_C(0x8C21), UINT16_C(0xC62D),
        UINT16_C(0x788F), UINT16_C(0xAA7F), UINT16_C(0x6928), UINT16_C(0x94B6),
        UINT16_C(0x5CC7), UINT16_C(0x8335), UINT16_C(0x52A6), UINT16_C(0x74E2),
        UINT16_C(0x4A3E), UINT16_C(0x68FE), UINT16_C(0x432B), UINT16_C(0x5EFD)
      }};

      std::int16_t index       = (std::int16_t) (((std::uint32_t) (a >> 27U) & 0xEU) + oddExpA);
      std::uint16_t     eps    = (std::uint16_t) (a >> 12);
      std::uint16_t     r0     = (std::uint16_t)
                                 (
                                                                       softfloat_approxRecipSqrt_1k0s[(std::size_t) index]
                                   - (std::uint16_t) ((std::uint32_t) (softfloat_approxRecipSqrt_1k1s[(std::size_t) index] * (std::uint32_t) eps) >> 20U)
                                 );
      std::uint32_t     ESqrR0 = (std::uint32_t) r0 * r0;

      if(!oddExpA)
      {
        ESqrR0 <<= 1U;
      }

      const std::uint32_t sigma0 = ~(std::uint32_t) (((std::uint32_t) ESqrR0 * (std::uint64_t) a) >> 23U);

      std::uint32_t r = (((std::uint32_t) r0) << 16U) + (std::uint32_t) ((r0 * (std::uint64_t) sigma0) >> 25U);

      const std::uint32_t sqrSigma0 = (std::uint32_t) ((std::uint64_t) ((std::uint64_t) sigma0 * sigma0) >> 32U);

      r += (std::uint32_t) ((std::uint64_t) ((std::uint32_t) ((std::uint32_t) ((r >> 1U) + (r >> 3U)) - ((std::uint32_t) r0 << 14U)) * (std::uint64_t) sqrSigma0) >> 48U);

      if(!(r & UINT32_C(0x80000000)))
      {
        r = UINT32_C(0x80000000);
      }

      return r;
    }

    static auto softfloat_normRoundPackToF64(bool sign, std::int16_t expA, std::uint64_t sig) -> std::uint64_t
    {
      const int_fast8_t shiftDist = (int_fast8_t) (detail::softfloat_countLeadingZeros64(sig) - 1U);

      expA = (std::int16_t) (expA - (std::int16_t) shiftDist);

      if((10 <= shiftDist) && ((std::uint32_t) expA < 0x7FDU))
      {
        const std::uint64_t uZ = detail::packToF64UI(sign, sig ? expA : 0, sig << (shiftDist - 10));

        return uZ;
      }
      else
      {
        return softfloat_roundPackToF64(sign, expA, sig << shiftDist);
      }
    }

    static auto softfloat_roundPackToF64(bool sign, std::int16_t expA, std::uint64_t sig) -> std::uint64_t
    {
      if(0x7FDU <= (std::uint16_t) expA)
      {
        if(expA < 0)
        {
          sig  = detail::softfloat_shiftRightJam64(sig, static_cast<std::uint_fast16_t>(-expA));
          expA = 0;
        }
      }

      const auto roundBits = static_cast<std::uint16_t>(sig & 0x3FFU);

      sig = (sig + 0x200U) >> 10U;

      sig &= (std::uint64_t) (~(std::uint64_t) (((roundBits ^ 0x200U) == 0U ? 1U : 0U) & 1U));

      if(!sig)
      {
        expA = 0;
      }

      const std::uint64_t uiZ = detail::packToF64UI(sign, expA, sig);

      return uiZ;
    }

    static auto softfloat_subMagsF64(std::uint64_t uiA, std::uint64_t uiB, bool signZ) -> std::uint64_t
    {
      std::uint64_t uiZ;
      std::int16_t  expZ;

      auto expA = detail::expF64UI(uiA);
      auto sigA = detail::fracF64UI(uiA);
      auto expB = detail::expF64UI(uiB);
      auto sigB = detail::fracF64UI(uiB);

      const std::int16_t expDiff = (std::int16_t) (expA - expB);

      if(!expDiff)
      {
        std::int64_t sigDiff = (std::int64_t) (sigA - sigB);

        if(!sigDiff)
        {
          uiZ = detail::packToF64UI(false, 0, 0);
        }
        else
        {
          if(expA)
          {
            --expA;
          }

          if(sigDiff < 0)
          {
            signZ   = (!signZ);
            sigDiff = -sigDiff;
          }

          std::int_fast8_t shiftDist = (std::int_fast8_t) (detail::softfloat_countLeadingZeros64((std::uint64_t) sigDiff) - 11U);

          expZ = (std::int16_t) (expA - shiftDist);

          if(expZ < 0)
          {
            shiftDist = (std::int_fast8_t) expA;

            expZ = 0;
          }

          uiZ = detail::packToF64UI(signZ, expZ, sigDiff << shiftDist);
        }
      }
      else
      {
        sigA <<= 10U;
        sigB <<= 10U;

        std::uint64_t sigZ;

        if(expDiff < 0)
        {
          signZ = (!signZ);

          sigA += ((expA != 0) ? UINT64_C(0x4000000000000000) : sigA);
          sigA  = detail::softfloat_shiftRightJam64(sigA, static_cast<std::uint_fast16_t>(-expDiff));

          sigB |= UINT64_C(0x4000000000000000);

          expZ = expB;
          sigZ = sigB - sigA;
        }
        else
        {
          sigB += ((expB != 0) ? UINT64_C(0x4000000000000000) : sigB);
          sigB  = detail::softfloat_shiftRightJam64(sigB, static_cast<std::uint_fast16_t>(expDiff));

          sigA |= UINT64_C(0x4000000000000000);

          expZ = expA;
          sigZ = sigA - sigB;
        }

        return softfloat_normRoundPackToF64(signZ, (std::int16_t) (expZ - 1), sigZ);
      }

      return uiZ;
    }

    #if !defined(SOFT_DOUBLE_DISABLE_IOSTREAM)
    template<typename char_type, typename traits_type>
    friend auto operator<<(std::basic_ostream<char_type, traits_type>& os, const soft_double& f) -> std::basic_ostream<char_type, traits_type>&
    {
      // Note that this output stream function relies on
      // the availability of 64-bit native double.

      std::basic_ostringstream<char_type, traits_type> ostr;

      // Copy all ostream settings from out to local ostr.
      ostr.flags(os.flags());
      ostr.imbue(os.getloc());
      ostr.precision(os.precision());

      static_cast<void>(ostr << static_cast<double>(f));

      return (os << ostr.str());
    }

    template<typename char_type, typename traits_type>
    friend auto operator>>(std::basic_istream<char_type, traits_type>& is, soft_double& f) -> std::basic_istream<char_type, traits_type>&
    {
      // Note that this input stream function relies on
      // the availability of 64-bit native double.

      double v;

      // Receive a floating-point number from the input stream.
      static_cast<void>(is >> v);

      // Subsequently make a fixed-point object from it.
      f = v;

      return is;
    }
    #endif // !SOFT_DOUBLE_DISABLE_IOSTREAM

    friend inline auto isfinite(soft_double x) -> bool { return (((isnan)(x) == false) && ((isinf)(x) == false)); }
    friend inline auto isnan   (soft_double x) -> bool { return  (x.my_value == my_value_quiet_NaN().my_value); }
    friend inline auto isinf   (soft_double x) -> bool { return ((x.my_value & my_value_infinity().my_value) == my_value_infinity().my_value); }

    friend inline auto fabs (soft_double x) -> soft_double { return soft_double((std::uint64_t) (x.my_value & UINT64_C(0x7FFFFFFFFFFFFFFF)), detail::nothing()); }
    friend inline auto sqrt (soft_double x) -> soft_double { return soft_double(f64_sqrt(x.my_value), detail::nothing()); }

    friend inline auto frexp(soft_double x, int* expptr) -> soft_double
    {
      const auto sign =                 detail::signF64UI(x.my_value);
      const auto expA = (std::int16_t) (detail::expF64UI (x.my_value) - INT16_C(0x3FE));
      const auto frac =                 detail::fracF64UI(x.my_value);

      if(expptr != nullptr)
      {
        *expptr = static_cast<int>(expA);
      }

      return soft_double(detail::packToF64UI(sign, INT16_C(0x3FE), frac), detail::nothing());
    }

    friend inline auto ldexp(soft_double x, int expval) -> soft_double
    {
      const int expA = (int) (detail::expF64UI(x.my_value) + expval);

      return soft_double((std::uint64_t) ((std::uint64_t) (x.my_value & (std::uint64_t) ~(UINT64_C(0x7FF) << 52U)) | (std::uint64_t) expA << 52U), detail::nothing());
    }

    friend inline auto floor(soft_double x) -> soft_double
    {
      soft_double result;

      if(x < 0)
      {
        const int64_t     xn = (int64_t) x;
        const soft_double fn = soft_double(xn);

        const bool is_pure_integer = (fn.my_value == x.my_value);

        result =
          (is_pure_integer ? fn : soft_double((int64_t) (x - soft_double::my_value_one())));
      }
      else
      {
        result = soft_double((int64_t)  x);
      }

      return result;
    }

    friend inline auto ceil(soft_double x) -> soft_double
    {
      soft_double result;

      if(x < 0)
      {
        result = soft_double((int64_t)  x);
      }
      else
      {
        const int64_t     xn = (int64_t) x;
        const soft_double fn = soft_double(xn);

        const bool is_pure_integer = (fn.my_value == x.my_value);

        result =
          (is_pure_integer ? fn : soft_double((int64_t) (x + soft_double::my_value_one())));
      }

      return result;
    }

    friend inline auto exp(soft_double x) -> soft_double
    {
      // PadeApproximant[Exp[x] - 1, {x, 0, {6, 6}}]
      // FullSimplify[%]
      //   (84 x (7920 + 240 x^2 + x^4))
      // / (665280 + x (-332640 + x (75600 + x (-10080 + x (840 + (-42 + x) x)))))

      const int n = (int) (x / soft_double::my_value_ln2());

      // Scale the argument such that Exp[x] = Exp[a + n * ln2].
      soft_double a = x - (n * soft_double::my_value_ln2());

      // Scale the argument yet again with division by 4.
      const int expA = (int) (detail::expF64UI(a.my_value) - 2);

      a.my_value &= (std::uint64_t) ~(UINT64_C(0x7FF) << 52U);
      a.my_value |= (std::uint64_t) expA << 52U;

      const soft_double a2 = a * a;

      // Use the small-argument Pade approximation having coefficients shown above.
      const soft_double top = (84 * a * (7920 + (240 + a2) * a2));
      const soft_double bot = (665280 + a * (-332640 + a * (75600 + a * (-10080 + a * (840 + (-42 + a) * a)))));

      soft_double result  = soft_double::my_value_one() + (top / bot);

      result *= result;
      result *= result;

      return ((n != 0) ? ldexp(result, n) : result);
    }

    friend inline auto log(soft_double x) -> soft_double
    {
      soft_double result;

      if(x < soft_double::my_value_one())
      {
        result = -log(soft_double::my_value_one() / x);
      }
      else if(x > soft_double::my_value_one())
      {
        // See J.F. Hart et al., "Computer Approximations" (Krieger Publishing 1978),
        // page 111 (with correction) log_e = zP(z^2) / Q(z^2), where the leading z
        // in the book is incorrect and coefficients are listed in LOGE 2688 on page 194.

        // P00 = -0.9812796124599728774775146E3;
        // P01 = +0.2098680741193909440902139E4;
        // P02 = -0.149596980292286125003062E4;
        // P03 = +0.39599373637329080057224E3;
        // P04 = -0.2857383397938418121918E2;
        // Q00 = -0.4906398062299865742258528E3;
        // Q01 = +0.1212886972673648778600404E4;
        // Q02 = -0.1054152597775536603834536E4;
        // Q03 = +0.37689507387121749065832E3;
        // Q04 = -0.4784212345106557147125E2;
        // Q05 = +0.1E1;

        // Scale the argument such that 1 <= a < 2.
        const std::int16_t n = (std::int16_t) (detail::expF64UI(x.my_value) - INT16_C(0x3FF));

        const soft_double a((std::uint64_t) ((std::uint64_t) (x.my_value & (std::uint64_t) ~(UINT64_C(0x7FF) << 52U)) | UINT64_C(0x3FF) << 52U), detail::nothing());

        const soft_double z  = (a - 1) / (a + 1);
        const soft_double z2 = z * z;

        const soft_double pz2 =  (((((     + soft_double(UINT64_C(0xC03C92E6C89EC9E1), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0x4078BFE6581C8213), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0xC0975FE114047504), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0x40A0655C8A1C19C3), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0xC08EAA3CA575191A), detail::nothing()));

        const soft_double qz2 = ((((((     + soft_double(UINT64_C(0x3FF0000000000000), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0xC047EBCAB384C2B9), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0x40778E5238FAC622), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0xC090789C42975D7D), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0x4092F38C42908712), detail::nothing()))
                                      * z2 + soft_double(UINT64_C(0xC07EAA3CA575191C), detail::nothing()));

        result  = ((z * pz2) / qz2);

        result += (n * soft_double::my_value_ln2());
      }
      else
      {
        result = soft_double::my_value_zero();
      }

      return result;
    }

    friend inline auto pow(soft_double x, soft_double a) -> soft_double
    {
      return exp(a * log(x));
    }

    template<typename UnsignedIntegralType,
             typename std::enable_if<(   (std::is_integral<UnsignedIntegralType>::value == true)
                                      && (std::is_unsigned<UnsignedIntegralType>::value == true))>::type const*>
    friend auto pow(soft_double x, UnsignedIntegralType u) -> soft_double
    {
      soft_double result;

      if     (u == 0U) { result = soft_double::my_value_one(); }
      else if(u == 1U) { result = x; }
      else if(u == 2U) { result = x * x; }
      else
      {
        using floating_point_type = soft_double;

        result = soft_double::my_value_one();

        floating_point_type y(x);

        for(std::uint32_t p_local = (std::uint32_t) u; p_local != 0U; p_local >>= 1U)
        {
          if((p_local & 1U) != 0U)
          {
            result *= y;
          }

          y *= y;
        }
      }

      return result;
    }

    template<typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed  <SignedIntegralType>::value == true))>::type const*>
    friend auto pow(soft_double x, SignedIntegralType n) -> soft_double
    {
      soft_double result;

      if     (n <  0) { result = soft_double::my_value_one() / pow(x, (SignedIntegralType) -n); }
      else if(n == 0) { result = soft_double::my_value_one(); }
      else if(n == 1) { result = x; }
      else if(n == 2) { result = x * x; }
      else
      {
        using floating_point_type = soft_double;

        result = soft_double::my_value_one();

        floating_point_type y(x);

        for(std::uint32_t p_local = (std::uint32_t) n; p_local != 0U; p_local >>= 1U)
        {
          if((p_local & 1U) != 0U)
          {
            result *= y;
          }

          y *= y;
        }
      }

      return result;
    }

    friend inline auto sin(soft_double x) -> soft_double;
    friend inline auto cos(soft_double x) -> soft_double;
    friend inline auto tan(soft_double x) -> soft_double;

    friend inline auto operator+(const soft_double& a, const soft_double& b) -> soft_double { return soft_double(f64_add(a.my_value, b.my_value), detail::nothing()); }
    friend inline auto operator-(const soft_double& a, const soft_double& b) -> soft_double { return soft_double(f64_sub(a.my_value, b.my_value), detail::nothing()); }
    friend inline auto operator*(const soft_double& a, const soft_double& b) -> soft_double { return soft_double(f64_mul(a.my_value, b.my_value), detail::nothing()); }
    friend inline auto operator/(const soft_double& a, const soft_double& b) -> soft_double { return soft_double(f64_div(a.my_value, b.my_value), detail::nothing()); }

    template<typename UnsignedIntegralType> friend auto operator+(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(u) += soft_double(n); }
    template<typename UnsignedIntegralType> friend auto operator-(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(u) -= soft_double(n); }
    template<typename UnsignedIntegralType> friend auto operator*(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(u) *= soft_double(n); }
    template<typename UnsignedIntegralType> friend auto operator/(const soft_double& u, UnsignedIntegralType n) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(u) /= soft_double(n); }

    template<typename UnsignedIntegralType> friend auto operator+(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(n) += u; }
    template<typename UnsignedIntegralType> friend auto operator-(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(n) -= u; }
    template<typename UnsignedIntegralType> friend auto operator*(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(n) *= u; }
    template<typename UnsignedIntegralType> friend auto operator/(UnsignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type { return soft_double(n) /= u; }

    template<typename SignedIntegralType> friend auto operator+(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(u) += soft_double(n); }
    template<typename SignedIntegralType> friend auto operator-(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(u) -= soft_double(n); }
    template<typename SignedIntegralType> friend auto operator*(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(u) *= soft_double(n); }
    template<typename SignedIntegralType> friend auto operator/(const soft_double& u, SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(u) /= soft_double(n); }

    template<typename SignedIntegralType> friend auto operator+(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(n) += u; }
    template<typename SignedIntegralType> friend auto operator-(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(n) -= u; }
    template<typename SignedIntegralType> friend auto operator*(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(n) *= u; }
    template<typename SignedIntegralType> friend auto operator/(SignedIntegralType n, const soft_double& u) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), soft_double>::type { return soft_double(n) /= u; }

    friend inline auto operator+(const soft_double& u, float f) -> soft_double { return soft_double(u) += soft_double(f); }
    friend inline auto operator-(const soft_double& u, float f) -> soft_double { return soft_double(u) -= soft_double(f); }
    friend inline auto operator*(const soft_double& u, float f) -> soft_double { return soft_double(u) *= soft_double(f); }
    friend inline auto operator/(const soft_double& u, float f) -> soft_double { return soft_double(u) /= soft_double(f); }

    friend inline auto operator+(float f, const soft_double& u) -> soft_double { return soft_double(f) += u; }
    friend inline auto operator-(float f, const soft_double& u) -> soft_double { return soft_double(f) -= u; }
    friend inline auto operator*(float f, const soft_double& u) -> soft_double { return soft_double(f) *= u; }
    friend inline auto operator/(float f, const soft_double& u) -> soft_double { return soft_double(f) /= u; }

    friend inline auto operator+(const soft_double& u, double f) -> soft_double { return soft_double(u) += soft_double(f); }
    friend inline auto operator-(const soft_double& u, double f) -> soft_double { return soft_double(u) -= soft_double(f); }
    friend inline auto operator*(const soft_double& u, double f) -> soft_double { return soft_double(u) *= soft_double(f); }
    friend inline auto operator/(const soft_double& u, double f) -> soft_double { return soft_double(u) /= soft_double(f); }

    friend inline auto operator+(double f, const soft_double& u) -> soft_double { return soft_double(f) += u; }
    friend inline auto operator-(double f, const soft_double& u) -> soft_double { return soft_double(f) -= u; }
    friend inline auto operator*(double f, const soft_double& u) -> soft_double { return soft_double(f) *= u; }
    friend inline auto operator/(double f, const soft_double& u) -> soft_double { return soft_double(f) /= u; }

    friend inline auto operator+(const soft_double& u, long double f) -> soft_double { return soft_double(u) += soft_double((double) f); }
    friend inline auto operator-(const soft_double& u, long double f) -> soft_double { return soft_double(u) -= soft_double((double) f); }
    friend inline auto operator*(const soft_double& u, long double f) -> soft_double { return soft_double(u) *= soft_double((double) f); }
    friend inline auto operator/(const soft_double& u, long double f) -> soft_double { return soft_double(u) /= soft_double((double) f); }

    friend inline auto operator+(long double f, const soft_double& u) -> soft_double { return soft_double((double) f) += u; }
    friend inline auto operator-(long double f, const soft_double& u) -> soft_double { return soft_double((double) f) -= u; }
    friend inline auto operator*(long double f, const soft_double& u) -> soft_double { return soft_double((double) f) *= u; }
    friend inline auto operator/(long double f, const soft_double& u) -> soft_double { return soft_double((double) f) /= u; }

    friend inline constexpr auto operator< (const soft_double& a, const soft_double& b) -> bool { return my_lt(a, b); }
    friend inline constexpr auto operator<=(const soft_double& a, const soft_double& b) -> bool { return my_le(a, b); }
    friend inline constexpr auto operator==(const soft_double& a, const soft_double& b) -> bool { return (a.my_value == b.my_value); }
    friend inline constexpr auto operator!=(const soft_double& a, const soft_double& b) -> bool { return (!(a == b)); }
    friend inline constexpr auto operator>=(const soft_double& a, const soft_double& b) -> bool { return (!(a <  b)); }
    friend inline constexpr auto operator> (const soft_double& a, const soft_double& b) -> bool { return (!(a <= b)); }

    template<typename UnsignedIntegralType> friend auto operator< (const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return soft_double::my_lt(a, soft_double(u)); }
    template<typename UnsignedIntegralType> friend auto operator<=(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return soft_double::my_le(a, soft_double(u)); }
    template<typename UnsignedIntegralType> friend auto operator==(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return  (a.my_value == soft_double(u).my_value); }
    template<typename UnsignedIntegralType> friend auto operator!=(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return ((a == soft_double(u)) == false); }
    template<typename UnsignedIntegralType> friend auto operator>=(const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return ((a <  soft_double(u)) == false); }
    template<typename UnsignedIntegralType> friend auto operator> (const soft_double& a, const UnsignedIntegralType u) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return ((a <= soft_double(u)) == false); }

    template<typename UnsignedIntegralType> friend auto operator< (const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return soft_double::my_lt(soft_double(u), a); }
    template<typename UnsignedIntegralType> friend auto operator<=(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return soft_double::my_le(soft_double(u), a); }
    template<typename UnsignedIntegralType> friend auto operator==(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return  (soft_double(u).my_value == a.my_value); }
    template<typename UnsignedIntegralType> friend auto operator!=(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return ((soft_double(u) == a) == false); }
    template<typename UnsignedIntegralType> friend auto operator>=(const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return ((soft_double(u) <  a) == false); }
    template<typename UnsignedIntegralType> friend auto operator> (const UnsignedIntegralType u, const soft_double& a) -> typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), bool>::type { return ((soft_double(u) <= a) == false); }

    template<typename SignedIntegralType> friend auto operator< (const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return soft_double::my_lt(a, soft_double(n)); }
    template<typename SignedIntegralType> friend auto operator<=(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return soft_double::my_le(a, soft_double(n)); }
    template<typename SignedIntegralType> friend auto operator==(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return  (a.my_value == soft_double(n).my_value); }
    template<typename SignedIntegralType> friend auto operator!=(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return ((a == soft_double(n)) == false); }
    template<typename SignedIntegralType> friend auto operator>=(const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return ((a <  soft_double(n)) == false); }
    template<typename SignedIntegralType> friend auto operator> (const soft_double& a, const SignedIntegralType n) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return ((a <= soft_double(n)) == false); }

    template<typename SignedIntegralType> friend auto operator< (const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return soft_double::my_lt(soft_double(n), a); }
    template<typename SignedIntegralType> friend auto operator<=(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return soft_double::my_le(soft_double(n), a); }
    template<typename SignedIntegralType> friend auto operator==(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return  (soft_double(n).my_value == a.my_value); }
    template<typename SignedIntegralType> friend auto operator!=(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return ((soft_double(n) == a) == false); }
    template<typename SignedIntegralType> friend auto operator>=(const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return ((soft_double(n) <  a) == false); }
    template<typename SignedIntegralType> friend auto operator> (const SignedIntegralType n, const soft_double& a) -> typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), bool>::type { return ((soft_double(n) <= a) == false); }

    friend inline auto operator< (const soft_double& a, const float f) -> bool { return soft_double::my_lt(a, soft_double(f)); }
    friend inline auto operator<=(const soft_double& a, const float f) -> bool { return soft_double::my_le(a, soft_double(f)); }
    friend inline auto operator==(const soft_double& a, const float f) -> bool { return  (a.my_value == soft_double(f).my_value); }
    friend inline auto operator!=(const soft_double& a, const float f) -> bool { return ((a == soft_double(f)) == false); }
    friend inline auto operator>=(const soft_double& a, const float f) -> bool { return ((a <  soft_double(f)) == false); }
    friend inline auto operator> (const soft_double& a, const float f) -> bool { return ((a <= soft_double(f)) == false); }

    friend inline auto operator< (const float f, const soft_double& a) -> bool { return soft_double::my_lt(soft_double(f), a); }
    friend inline auto operator<=(const float f, const soft_double& a) -> bool { return soft_double::my_le(soft_double(f), a); }
    friend inline auto operator==(const float f, const soft_double& a) -> bool { return  (soft_double(f).my_value == a.my_value); }
    friend inline auto operator!=(const float f, const soft_double& a) -> bool { return ((soft_double(f) == a) == false); }
    friend inline auto operator>=(const float f, const soft_double& a) -> bool { return ((soft_double(f) <  a) == false); }
    friend inline auto operator> (const float f, const soft_double& a) -> bool { return ((soft_double(f) <= a) == false); }

    friend inline auto operator< (const soft_double& a, const double f) -> bool { return soft_double::my_lt(a, soft_double(f)); }
    friend inline auto operator<=(const soft_double& a, const double f) -> bool { return soft_double::my_le(a, soft_double(f)); }
    friend inline auto operator==(const soft_double& a, const double f) -> bool { return  (a.my_value == soft_double(f).my_value); }
    friend inline auto operator!=(const soft_double& a, const double f) -> bool { return ((a == soft_double(f)) == false); }
    friend inline auto operator>=(const soft_double& a, const double f) -> bool { return ((a <  soft_double(f)) == false); }
    friend inline auto operator> (const soft_double& a, const double f) -> bool { return ((a <= soft_double(f)) == false); }

    friend inline auto operator< (const double f, const soft_double& a) -> bool { return soft_double::my_lt(soft_double(f), a); }
    friend inline auto operator<=(const double f, const soft_double& a) -> bool { return soft_double::my_le(soft_double(f), a); }
    friend inline auto operator==(const double f, const soft_double& a) -> bool { return  (soft_double(f).my_value == a.my_value); }
    friend inline auto operator!=(const double f, const soft_double& a) -> bool { return ((soft_double(f) == a) == false); }
    friend inline auto operator>=(const double f, const soft_double& a) -> bool { return ((soft_double(f) <  a) == false); }
    friend inline auto operator> (const double f, const soft_double& a) -> bool { return ((soft_double(f) <= a) == false); }

    friend inline auto operator< (const soft_double& a, const long double f) -> bool { return soft_double::my_lt(a, soft_double((double) f)); }
    friend inline auto operator<=(const soft_double& a, const long double f) -> bool { return soft_double::my_le(a, soft_double((double) f)); }
    friend inline auto operator==(const soft_double& a, const long double f) -> bool { return  (a.my_value == soft_double((double) f).my_value); }
    friend inline auto operator!=(const soft_double& a, const long double f) -> bool { return ((a == soft_double((double) f)) == false); }
    friend inline auto operator>=(const soft_double& a, const long double f) -> bool { return ((a <  soft_double((double) f)) == false); }
    friend inline auto operator> (const soft_double& a, const long double f) -> bool { return ((a <= soft_double((double) f)) == false); }

    friend inline auto operator< (const long double f, const soft_double& a) -> bool { return soft_double::my_lt(soft_double((double) f), a); }
    friend inline auto operator<=(const long double f, const soft_double& a) -> bool { return soft_double::my_le(soft_double((double) f), a); }
    friend inline auto operator==(const long double f, const soft_double& a) -> bool { return  (soft_double((double) f).my_value == a.my_value); }
    friend inline auto operator!=(const long double f, const soft_double& a) -> bool { return ((soft_double((double) f) == a) == false); }
    friend inline auto operator>=(const long double f, const soft_double& a) -> bool { return ((soft_double((double) f) <  a) == false); }
    friend inline auto operator> (const long double f, const soft_double& a) -> bool { return ((soft_double((double) f) <= a) == false); }
  };

  namespace detail {

  inline auto sin_pade(soft_double x) -> soft_double
  {
    // PadeApproximant[Sin[x], {x, 0, {7,7}}]
    // FullSimplify[%]

    //    x (11511339840 - 1640635920 x^2 + 52785432 x^4 - 479249 x^6)
    // / (7 ( 1644477120  +  39702960 x^2 +   453960 x^4 +   2623 x^6))

    static const soft_double coef_sin_top_0(INT64_C(+11511339840));
    static const soft_double coef_sin_top_1(INT32_C(-1640635920));
    static const soft_double coef_sin_top_2(INT32_C(+52785432));
    static const soft_double coef_sin_top_3(INT32_C(-479249));

    static const soft_double coef_sin_bot_0(UINT32_C(+1644477120));
    static const soft_double coef_sin_bot_1(UINT32_C(+39702960));
    static const soft_double coef_sin_bot_2(UINT32_C(+453960));
    static const soft_double coef_sin_bot_3(UINT32_C(+2623));


    const soft_double x2(x * x);

    const soft_double top = ((((     + coef_sin_top_3)
                                * x2 + coef_sin_top_2)
                                * x2 + coef_sin_top_1)
                                * x2 + coef_sin_top_0);

    const soft_double bot = ((((     + coef_sin_bot_3)
                                * x2 + coef_sin_bot_2)
                                * x2 + coef_sin_bot_1)
                                * x2 + coef_sin_bot_0);

    return (x * top) / (bot * 7);
  }

  inline auto cos_pade(soft_double x) -> soft_double
  {
    // PadeApproximant[Cos[x] - 1, {x, 0, {8,6}}]
    // FullSimplify[%]

    //   (x^2 (-5491886400 + 346666320 x^2 - 7038360 x^4 + 45469 x^6))
    // / (24  (  457657200 +   9249240 x^2 +   86030 x^4 +   389 x^6))

    static const soft_double coef_cos_top_0(INT64_C(-5491886400));
    static const soft_double coef_cos_top_1(INT32_C(+346666320));
    static const soft_double coef_cos_top_2(INT32_C(-7038360));
    static const soft_double coef_cos_top_3(INT32_C(+45469));

    static const soft_double coef_cos_bot_0(UINT32_C(457657200));
    static const soft_double coef_cos_bot_1(UINT32_C(9249240));
    static const soft_double coef_cos_bot_2(UINT32_C(86030));
    static const soft_double coef_cos_bot_3(UINT32_C(389));

    const soft_double x2(x * x);

    const soft_double top = ((((     + coef_cos_top_3)
                                * x2 + coef_cos_top_2)
                                * x2 + coef_cos_top_1)
                                * x2 + coef_cos_top_0);

    const soft_double bot = ((((     + coef_cos_bot_3)
                                * x2 + coef_cos_bot_2)
                                * x2 + coef_cos_bot_1)
                                * x2 + coef_cos_bot_0);

    return 1U + ((x2 * top) / (bot * 24));
  }

  }

  inline auto sin(soft_double x) -> soft_double
  {
    soft_double s;

    if(x < 0)
    {
      s = -sin(-x);
    }
    else if(x > 0)
    {
      // Remove even multiples of pi.

      bool b_negate_sin = false;

      const std::uint32_t n_pi = (std::uint32_t) (x / soft_double::my_value_pi());

      if(n_pi != 0U)
      {
        x -= (soft_double::my_value_pi() * n_pi);

        if((n_pi % 2U) != 0)
        {
          b_negate_sin = (!b_negate_sin);
        }
      }

      // Check if the reduced argument is very close to pi/2.
      const soft_double delta_pi_half  = soft_double::my_value_pi_half() - x;
      const bool        b_near_pi_half = (fabs(delta_pi_half) < soft_double(UINT64_C(0x3FE0C152382D7366), detail::nothing()));

      if(b_near_pi_half)
      {
        // PadeApproximant[Hypergeometric0F1[1/2, -(dx^2)/4], {dx, 0, {6, 6}}]
        // FullSimplify[%]
        //   (39251520 - 18471600 dx^2 + 1075032 dx^4 - 14615 dx^6)
        // / (39251520 +  1154160 dx^2 +   16632 dx^4 +   127 dx^6)

        static const soft_double coef_top_0(INT32_C(+39251520));
        static const soft_double coef_top_1(INT32_C(-18471600));
        static const soft_double coef_top_2(INT32_C(+1075032));
        static const soft_double coef_top_3(INT32_C(-14615));

        static const soft_double coef_bot_0(UINT32_C(39251520));
        static const soft_double coef_bot_1(UINT32_C(1154160));
        static const soft_double coef_bot_2(UINT32_C(16632));
        static const soft_double coef_bot_3(UINT32_C(127));

        const soft_double x2(delta_pi_half * delta_pi_half);

        const soft_double top = ((((     + coef_top_3)
                                    * x2 + coef_top_2)
                                    * x2 + coef_top_1)
                                    * x2 + coef_top_0);

        const soft_double bot = ((((     + coef_bot_3)
                                    * x2 + coef_bot_2)
                                    * x2 + coef_bot_1)
                                    * x2 + coef_bot_0);

        s = top / bot;
      }
      else
      {
        // Reduce the argument to 0 <= x <= pi/2.
        if(x > soft_double::my_value_pi_half())
        {
          x = soft_double::my_value_pi() - x;
        }

        if(x > soft_double(UINT64_C(0x3FE0C152382D7366), detail::nothing()))
        {
          // x is larger than pi/6, scale by 9.
          x /= 9U;

          s = detail::sin_pade(x);

          s = (s * 3U) - (((s * s) * s) * 4U);
          s = (s * 3U) - (((s * s) * s) * 4U);
        }
        else if(x > soft_double(UINT64_C(0x3FC657184AE74487), detail::nothing()))
        {
          // x is larger than pi/18, scale by 3.
          x /= 3U;

          s = detail::sin_pade(x);

          s = (s * 3U) - (((s * s) * s) * 4U);
        }
        else
        {
          s = detail::sin_pade(x);
        }
      }

      if(b_negate_sin)
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

  inline auto cos(soft_double x) -> soft_double
  {
    soft_double c;

    if(x < 0)
    {
      c = cos(-x);
    }
    else if(x > 0)
    {
      // Remove even multiples of pi.

      bool b_negate_cos = false;

      const std::uint32_t n_pi = (std::uint32_t) (x / soft_double::my_value_pi());

      if(n_pi != 0U)
      {
        x -= (soft_double::my_value_pi() * n_pi);

        if((n_pi % 2U) != 0)
        {
          b_negate_cos = (!b_negate_cos);
        }
      }

      // Check if the reduced argument is very close to pi/2.
      const soft_double delta_pi_half  = soft_double::my_value_pi_half() - x;
      const bool        b_near_pi_half = (fabs(delta_pi_half) < soft_double(UINT64_C(0x3FE0C152382D7366), detail::nothing()));

      if(b_near_pi_half)
      {
        // PadeApproximant[Hypergeometric0F1[3/2, -(dx^2)/4], {dx, 0, {6, 6}}]
        // FullSimplify[%]
        //      (11511339840 - 1640635920 dx^2 + 52785432 dx^4 - 479249 dx^6)
        // / (7 ( 1644477120 +   39702960 dx^2 +   453960 dx^4 +   2623 dx^6))

        // This is the same approximation as the one found in detail::sin_pade().

        c = detail::sin_pade(delta_pi_half);
      }
      else
      {
        // Reduce the argument to 0 <= x <= pi/2.
        if(x > soft_double::my_value_pi_half())
        {
          x = soft_double::my_value_pi() - x;

          b_negate_cos = (!b_negate_cos);
        }

        if(x > soft_double(UINT64_C(0x3FE0C152382D7366), detail::nothing()))
        {
          // x is larger than pi/6, scale by 9.
          x /= 9U;

          c = detail::cos_pade(x);

          c = (((c * c) * c) * 4U) - (c * 3U);
          c = (((c * c) * c) * 4U) - (c * 3U);
        }
        else if(x > soft_double(UINT64_C(0x3FC657184AE74487), detail::nothing()))
        {
          // x is larger than pi/18, scale by 3.
          x /= 3U;

          c = detail::cos_pade(x);

          c = (((c * c) * c) * 4U) - (c * 3U);
        }
        else
        {
          c = detail::cos_pade(x);
        }
      }

      if(b_negate_cos)
      {
        c = -c;
      }
    }
    else
    {
      c = soft_double::my_value_one();
    }

    return c;
  }

  inline auto tan(const soft_double x) -> soft_double
  {
    return sin(x) / cos(x);
  }

  inline auto sinh(soft_double x) -> soft_double
  {
    const soft_double ep = exp(x);

    return (ep - (1 / ep)) / 2;
  }

  inline auto cosh(soft_double x) -> soft_double
  {
    const soft_double ep = exp(x);

    return (ep + (1 / ep)) / 2;
  }

  inline auto tanh(soft_double x) -> soft_double
  {
    const soft_double ep = exp(x);
    const soft_double em = 1 / ep;

    return (ep - em) / (ep + em);
  }

  using float64_t = soft_double;

  #if(__cplusplus >= 201703L)
  } // namespace math::softfloat
  #else
  } // namespace softfloat
  } // namespace math
  #endif

  namespace std {

  // Specialization of numeric_limits for soft_double.
  template<>
  class numeric_limits<math::softfloat::soft_double>
  {
  public:
    static constexpr bool               is_specialized    = true;
    static constexpr float_denorm_style has_denorm        = denorm_present;
    static constexpr bool               has_denorm_loss   = false;
    static constexpr bool               has_infinity      = true;
    static constexpr bool               has_quiet_NaN     = true;
    static constexpr bool               has_signaling_NaN = false;
    static constexpr bool               is_bounded        = true;
    static constexpr bool               is_iec559         = false;
    static constexpr bool               is_integer        = false;
    static constexpr bool               is_exact          = false;
    static constexpr bool               is_signed         = true;
    static constexpr bool               is_modulo         = false;
    static constexpr bool               traps             = false;
    static constexpr bool               tinyness_before   = false;
    static constexpr float_round_style  round_style       = round_to_nearest;
    static constexpr int                radix             = 2;

    static constexpr int                digits            = 53;
    static constexpr int                digits10          = 15;
    static constexpr int                max_digits10      = 17;
    static constexpr int                max_exponent      = 1024;
    static constexpr int                max_exponent10    = 308;
    static constexpr int                min_exponent      = -1021;
    static constexpr int                min_exponent10    = -307;

    static constexpr auto (min)        () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_min(); }
    static constexpr auto (max)        () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_max(); }
    static constexpr auto lowest       () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_lowest(); }
    static constexpr auto epsilon      () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_epsilon(); }
    static constexpr auto round_error  () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_round_error(); }
    static constexpr auto denorm_min   () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_denorm_min(); }
    static constexpr auto infinity     () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_infinity(); }
    static constexpr auto quiet_NaN    () noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_quiet_NaN(); }
    static constexpr auto signaling_NaN() noexcept -> math::softfloat::soft_double { return math::softfloat::soft_double::my_value_signaling_NaN(); }
  };

  } // namespace std

#endif // SOFT_DOUBLE_2020_10_27_H
