///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

// This work uses (significantly) modified parts of the work mentioned below.

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

#ifndef SOFT_DOUBLE_2020_10_27_H_
  #define SOFT_DOUBLE_2020_10_27_H_

  #include <array>
  #include <cstdint>
  #include <cstddef>
  #include <limits>
  #include <type_traits>

  namespace math { namespace softfloat {

  // Forward declaration the math::softfloat::soft_double class.
  class soft_double;

  } }

  namespace std {

  // Forward declaration of the specialization of numeric_limits for soft_double.
  template<>
  class numeric_limits<math::softfloat::soft_double>;

  }

  namespace math { namespace softfloat {

  namespace detail {

  struct uint128_as_struct  { uint64_t v0, v64; };
  struct uint64_extra       { uint64_t extra, v; };

  constexpr bool     signF32UI(uint32_t a) { return ((bool) ((uint32_t) (a)>>31)); }
  constexpr int16_t  expF32UI (uint32_t a) { return ((int16_t) ((a)>>23) & 0xFF); }
  constexpr uint32_t fracF32UI(uint32_t a) { return ((a) & UINT32_C(0x007FFFFF)); }

  constexpr bool     signF64UI(uint64_t a) { return ((bool) ((uint64_t) (a)>>63)); }
  constexpr int16_t  expF64UI (uint64_t a) { return ((int16_t) ((a)>>52) & 0x7FF); }
  constexpr uint64_t fracF64UI(uint64_t a) { return ((a) & UINT64_C(0x000FFFFFFFFFFFFF)); }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr uint64_t packToF64UI(bool sign, IntegralTypeExp exp, IntegralTypeSig sig)
  {
    return ((uint64_t) ((uint64_t) (((uint64_t) (sign ? 1ULL : 0ULL))<<63) + (uint64_t) (((uint64_t) exp)<<52) + (uint64_t) sig));
  }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr uint32_t packToF32UI(bool sign, IntegralTypeExp exp, IntegralTypeSig sig)
  {
    return ((uint32_t) ((uint32_t) (((uint32_t) (sign ? 1ULL : 0ULL))<<31) + (uint32_t) (((uint32_t) exp)<<23) + (uint32_t) sig));
  }

  /*----------------------------------------------------------------------------
  | Shifts 'a' right by the number of bits given in 'dist', which must be in
  | the range 1 to 63.  If any nonzero bits are shifted off, they are "jammed"
  | into the least-significant bit of the shifted value by setting the least-
  | significant bit to 1.  This shifted-and-jammed value is returned.
  *----------------------------------------------------------------------------*/
  constexpr uint64_t softfloat_shortShiftRightJam64(uint64_t a, uint_fast8_t dist)
  {
    return a >> dist | ((a & (((uint64_t) 1 << dist) - 1)) != 0 ? 1U : 0U);
  }

  template<typename UnsignedIntegralType>
  inline constexpr typename std::enable_if<   (std::is_integral<UnsignedIntegralType>::value == true)
                                           && (std::is_unsigned<UnsignedIntegralType>::value == true), UnsignedIntegralType>::type
  negate(UnsignedIntegralType u)
  {
    return (UnsignedIntegralType) (((UnsignedIntegralType) ~u) + 1U);
  }

  template<typename SignedIntegralType>
  inline constexpr typename std::enable_if<   (std::is_integral<SignedIntegralType>::value == true)
                                           && (std::is_signed  <SignedIntegralType>::value == true), SignedIntegralType>::type
  negate(SignedIntegralType n)
  {
    return (SignedIntegralType) -n;
  }

  /*----------------------------------------------------------------------------
  | Shifts 'a' right by the number of bits given in 'dist', which must not
  | be zero.  If any nonzero bits are shifted off, they are "jammed" into the
  | least-significant bit of the shifted value by setting the least-significant
  | bit to 1.  This shifted-and-jammed value is returned.
  *----------------------------------------------------------------------------*/
  constexpr uint32_t softfloat_shiftRightJam32(uint32_t a, uint16_t dist)
  {
    return (dist < 31) ? a >> dist | ((uint32_t)(a << (negate(dist) & 31)) != 0 ? 1U : 0U) : (a != 0 ? 1U : 0U);
  }

  /*----------------------------------------------------------------------------
  | Shifts 'a' right by the number of bits given in 'dist', which must not
  | be zero.
  *----------------------------------------------------------------------------*/
  constexpr uint64_t softfloat_shiftRightJam64(uint64_t a, uint32_t dist)
  {
    return (dist < 63) ? a >> dist | ((uint64_t)(a << (negate(dist) & 63)) != 0 ? 1U : 0U) : (a != 0 ? 1U : 0U);
  }

  /*----------------------------------------------------------------------------
  | A constant table that translates an 8-bit unsigned integer (the array index)
  | into the number of leading 0 bits before the most-significant 1 of that
  | integer.  For integer zero (index 0), the corresponding table element is 8.
  *----------------------------------------------------------------------------*/
  constexpr uint_fast8_t softfloat_countLeadingZeros8_z_table(const uint_fast8_t index)
  {
    return ((index < 0x1U) ? 4U :
           ((index < 0x2U) ? 3U :
           ((index < 0x4U) ? 2U :
           ((index < 0x8U) ? 1U : 0U))));
  }

  constexpr uint_fast8_t softfloat_countLeadingZeros8(const uint_fast8_t index)
  {
    return (index < 0x10U) ? 4U + softfloat_countLeadingZeros8_z_table(index &  0xFU)
                           :      softfloat_countLeadingZeros8_z_table(index >>   4U);
  }

  constexpr uint_fast8_t softfloat_countLeadingZeros16(uint16_t a)
  {
    return (a < UINT16_C(0x100)) ? 8U + softfloat_countLeadingZeros8((uint_fast8_t) a)
                                 :      softfloat_countLeadingZeros8((uint_fast8_t) (a >> 8U));
  }

  constexpr uint_fast8_t softfloat_countLeadingZeros32(uint32_t a)
  {
    // TBD: Finding MSB to count leading zeros can probably be done more efficiently.
    return (a < UINT32_C(0x10000)) ? 16U + softfloat_countLeadingZeros16((uint16_t) a)
                                   :       softfloat_countLeadingZeros16((uint16_t) (a >> 16U));
  }

  /*----------------------------------------------------------------------------
  | Returns the number of leading 0 bits before the most-significant 1 bit of
  | 'a'.  If 'a' is zero, 64 is returned.
  *----------------------------------------------------------------------------*/
  constexpr uint_fast8_t softfloat_countLeadingZeros64(uint64_t a)
  {
    // TBD: Finding MSB to count leading zeros can probably be done more efficiently.
    return (a < UINT64_C(0x100000000)) ? 32U + softfloat_countLeadingZeros32((uint32_t) a)
                                       :       softfloat_countLeadingZeros32((uint32_t) (a >> 32U));
  }

  /*----------------------------------------------------------------------------
  | Returns an approximation to the reciprocal of the number represented by 'a',
  | where 'a' is interpreted as an unsigned fixed-point number with one integer
  | bit and 31 fraction bits.
  *----------------------------------------------------------------------------*/
  constexpr uint32_t softfloat_approxRecip32_1(uint32_t a)
  {
    return (uint32_t) (UINT64_C(0x7FFFFFFFFFFFFFFF) / a);
  }

  /*----------------------------------------------------------------------------
  | Shifts the 128 bits formed by concatenating 'a' and 'extra' right by 64
  | _plus_ the number of bits given in 'dist', which must not be zero.  This
  | shifted value is at most 64 nonzero bits and is returned in the 'v' field
  | of the 'struct uint64_extra' result.
  *----------------------------------------------------------------------------*/
  constexpr struct uint64_extra softfloat_shiftRightJam64Extra(uint64_t a, uint64_t extra, uint32_t dist)
  {
    return
    {
      (dist < 64) ? (a << (negate(dist) & 63)) | (extra != 0U ? 1U : 0U) : ((dist == 64) ? a : (a != 0U ? 1U : 0U))  | (extra != 0U ? 1U : 0U),
      (dist < 64) ?  a >> dist                                           : 0U
    };
  }

  template<typename BuiltinFloatType,
           typename ExactUnsignedIntegralType = typename std::conditional<std::is_same<BuiltinFloatType, float>::value, uint32_t, uint64_t>::type>
  union uz_type
  {
    static_assert((   std::is_same<BuiltinFloatType, float>::value
                   || std::is_same<BuiltinFloatType, double>::value),
                  "Error: This template is intended for either built-in float or double, but not for any other type(s)");

    using float_type    = BuiltinFloatType;
    using unsigned_type = ExactUnsignedIntegralType;

    const float_type    my_f;
    const unsigned_type my_u;

    constexpr uz_type(float_type    f) : my_f(f) { }
    constexpr uz_type(unsigned_type u) : my_u(u) { }
  };

  struct nothing { };

  }

  class soft_double;

  inline soft_double operator+(const soft_double& a, const soft_double& b);
  inline soft_double operator-(const soft_double& a, const soft_double& b);
  inline soft_double operator*(const soft_double& a, const soft_double& b);
  inline soft_double operator/(const soft_double& a, const soft_double& b);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral<UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type
  operator+(const soft_double& u, UnsignedIntegralType n);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral<UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type
  operator-(const soft_double& u, UnsignedIntegralType n);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral<UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type
  operator*(const soft_double& u, UnsignedIntegralType n);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral<UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned<UnsignedIntegralType>::value == true), soft_double>::type
  operator/(const soft_double& u, UnsignedIntegralType n);

  inline soft_double operator+(const soft_double& u, float f);
  inline soft_double operator-(const soft_double& u, float f);
  inline soft_double operator*(const soft_double& u, float f);
  inline soft_double operator/(const soft_double& u, float f);

  inline soft_double operator+(float f, const soft_double& u);
  inline soft_double operator-(float f, const soft_double& u);
  inline soft_double operator*(float f, const soft_double& u);
  inline soft_double operator/(float f, const soft_double& u);

  inline soft_double operator+(const soft_double&, double);
  inline soft_double operator-(const soft_double&, double);
  inline soft_double operator*(const soft_double&, double);
  inline soft_double operator/(const soft_double&, double);

  inline soft_double operator+(double, const soft_double&);
  inline soft_double operator-(double, const soft_double&);
  inline soft_double operator*(double, const soft_double&);
  inline soft_double operator/(double, const soft_double&);

  inline soft_double operator+(const soft_double& u, long double);
  inline soft_double operator-(const soft_double& u, long double);
  inline soft_double operator*(const soft_double& u, long double);
  inline soft_double operator/(const soft_double& u, long double);

  inline soft_double operator+(long double, const soft_double&);
  inline soft_double operator-(long double, const soft_double&);
  inline soft_double operator*(long double, const soft_double&);
  inline soft_double operator/(long double, const soft_double&);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator+(const soft_double& u, SignedIntegralType n);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator-(const soft_double& u, SignedIntegralType n);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator*(const soft_double& u, SignedIntegralType n);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator/(const soft_double& u, SignedIntegralType n);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
  operator+(UnsignedIntegralType n, const soft_double& u);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
  operator-(UnsignedIntegralType n, const soft_double& u);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
  operator*(UnsignedIntegralType n, const soft_double& u);

  template<typename UnsignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                 && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
  operator/(UnsignedIntegralType n, const soft_double& u);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator+(SignedIntegralType n, const soft_double& u);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator-(SignedIntegralType n, const soft_double& u);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator*(SignedIntegralType n, const soft_double& u);

  template<typename SignedIntegralType>
  inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                 && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
  operator/(SignedIntegralType n, const soft_double& u);

  inline constexpr bool operator< (const soft_double& a, const soft_double& b);
  inline constexpr bool operator<=(const soft_double& a, const soft_double& b);
  inline constexpr bool operator==(const soft_double& a, const soft_double& b);
  inline constexpr bool operator!=(const soft_double& a, const soft_double& b);
  inline constexpr bool operator>=(const soft_double& a, const soft_double& b);
  inline constexpr bool operator> (const soft_double& a, const soft_double& b);

  inline bool        isnan   (const soft_double x);
  inline bool        isinf   (const soft_double x);
  inline bool        isfinite(const soft_double x);
  inline soft_double fabs    (const soft_double x);
  inline soft_double sqrt    (const soft_double x);

  class soft_double final
  {
    static_assert(sizeof(float) == 4U,
                  "Error: This template is designed for 4 byte built-in float");
  public:
    using representation_type = uint64_t;

    soft_double() { }

    template<typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed  <SignedIntegralType>::value == true)
                                      && (std::numeric_limits<SignedIntegralType>::digits <= std::numeric_limits<std::int32_t>::digits))>::type const* = nullptr>
    constexpr soft_double(const SignedIntegralType n) : my_value(my__i32_to_f64((std::int32_t) n)) { }

    constexpr soft_double(const std::int64_t n) : my_value(my__i64_to_f64((std::int64_t) n)) { }

    template<typename UnsignedIntegralType,
             typename std::enable_if<(   (std::is_integral<UnsignedIntegralType>::value == true)
                                      && (std::is_unsigned<UnsignedIntegralType>::value == true)
                                      && (std::numeric_limits<UnsignedIntegralType>::digits <= std::numeric_limits<std::uint32_t>::digits))>::type const* = nullptr>
    constexpr soft_double(const UnsignedIntegralType u) : my_value(my_ui32_to_f64((std::uint32_t) u)) { }

    constexpr soft_double(const std::uint64_t u) : my_value(my_ui64_to_f64((std::uint64_t) u)) { }

    constexpr soft_double(const float f)
      : my_value
        (
          ((detail::expF32UI (detail::uz_type<float>(f).my_u) == 0) && (detail::fracF32UI(detail::uz_type<float>(f).my_u) == 0U))
            ? detail::packToF64UI(detail::signF32UI(detail::uz_type<float>(f).my_u), 0, 0)
            : detail::packToF64UI(detail::signF32UI(detail::uz_type<float>(f).my_u), detail::expF32UI (detail::uz_type<float>(f).my_u) + 0x380, (uint64_t) detail::fracF32UI(detail::uz_type<float>(f).my_u) << 29)
        ) { }

    constexpr soft_double(const double) = delete;
    constexpr soft_double(const long double) = delete;

    constexpr soft_double(const soft_double& other) noexcept
      : my_value(other.my_value) { }

    constexpr soft_double(const soft_double&& other) noexcept
      : my_value(other.my_value) { }

    constexpr soft_double(std::uint64_t n, const detail::nothing&) noexcept
      : my_value((std::uint64_t) n) { }

    ~soft_double() = default;

    soft_double& operator=(const soft_double& other)
    {
      if(this != &other)
      {
        my_value = other.my_value;
      }

      return *this;
    }

    soft_double& operator=(soft_double&& other)
    {
      my_value = other.my_value;

      return *this;
    }

          representation_type&  representation()       { return my_value; }
    const representation_type&  representation() const { return my_value; }
    const representation_type& crepresentation() const { return my_value; }

    static constexpr representation_type get_rep(soft_double a) { return a.my_value; }

    explicit operator std::int64_t () const { return (std::int64_t)  f64_to__i64(my_value); }
    explicit operator std::uint64_t() const { return (std::uint64_t) f64_to_ui64(my_value); }

    explicit operator std::int32_t () const { return (std::int32_t)  f64_to__i32(my_value); }
    explicit operator std::uint32_t() const { return (std::uint32_t) f64_to_ui32(my_value); }

    explicit operator float() const
    {
      return f64_to_f32(my_value);
    }

    soft_double& operator+=(const soft_double& other) { my_value = f64_add(my_value, other.my_value); return *this; }
    soft_double& operator-=(const soft_double& other) { my_value = f64_sub(my_value, other.my_value); return *this; }
    soft_double& operator*=(const soft_double& other) { my_value = f64_mul(my_value, other.my_value); return *this; }
    soft_double& operator/=(const soft_double& other) { my_value = f64_div(my_value, other.my_value); return *this; }

    // Operators pre-increment and pre-decrement.
    soft_double& operator++() { return *this += my_value_one(); }
    soft_double& operator--() { return *this -= my_value_one(); }

    // Operators post-increment and post-decrement.
    soft_double operator++(int) { const soft_double w(*this); static_cast<void>(++(*this)); return w; }
    soft_double operator--(int) { const soft_double w(*this); static_cast<void>(--(*this)); return w; }

    const soft_double& operator+() const { return *this; }
          soft_double  operator-() const { return soft_double(my_value ^ (uint64_t) (1ULL << 63U), detail::nothing()); }

    static constexpr soft_double my_value_zero() { return soft_double(UINT64_C(0),                   detail::nothing()); }
    static constexpr soft_double my_value_one () { return soft_double(UINT64_C(0x3FF0000000000000),  detail::nothing()); }
    static constexpr soft_double my_value_pi  () { return soft_double(UINT64_C(4614256656552045848), detail::nothing()); }

    static constexpr soft_double my_value_min()           { return soft_double(UINT64_C(4503599627370496),    detail::nothing()); }
    static constexpr soft_double my_value_max()           { return soft_double(UINT64_C(9218868437227405311), detail::nothing()); }
    static constexpr soft_double my_value_lowest()        { return soft_double(UINT64_C(18442240474082181119),detail::nothing()); }
    static constexpr soft_double my_value_epsilon()       { return soft_double(UINT64_C(4372995238176751616), detail::nothing()); }
    static constexpr soft_double my_value_round_error()   { return soft_double(UINT64_C(4602678819172646912), detail::nothing()); }
    static constexpr soft_double my_value_denorm_min()    { return soft_double(UINT64_C(1),                   detail::nothing()); }
    static constexpr soft_double my_value_infinity()      { return soft_double(UINT64_C(0x7FF0000000000000),  detail::nothing()); }
    static constexpr soft_double my_value_quiet_NaN()     { return soft_double(UINT64_C(0xFFF8000000000000),  detail::nothing()); }
    static constexpr soft_double my_value_signaling_NaN() { return soft_double(UINT64_C(0x7FF8000000000000),  detail::nothing()); }

  private:
    representation_type my_value;

    static constexpr bool my_le(const soft_double& a, const soft_double& b)
    {
      return (detail::signF64UI(a.my_value) != detail::signF64UI(b.my_value))
               ? (detail::signF64UI(a.my_value) || !((a.my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF)))
               : (a.my_value == b.my_value) || (detail::signF64UI(a.my_value) ^ (a.my_value < b.my_value));
    }

    static constexpr bool my_lt(const soft_double& a, const soft_double& b)
    {
      return (detail::signF64UI(a.my_value) != detail::signF64UI(b.my_value))
               ? detail::signF64UI(a.my_value) && ((a.my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF))
               : (a.my_value != b.my_value) && (detail::signF64UI(a.my_value) ^ (a.my_value < b.my_value));
    }

    static uint64_t f64_add(const uint64_t a, const uint64_t b)
    {
      const bool signA = detail::signF64UI(a);
      const bool signB = detail::signF64UI(b);

      if(signA == signB)
      {
        return softfloat_addMagsF64(a, b, signA);
      }
      else
      {
        return softfloat_subMagsF64(a, b, signA);
      }
    }

    static uint64_t f64_sub(const uint64_t a, const uint64_t b)
    {
      const bool signA = detail::signF64UI(a);
      const bool signB = detail::signF64UI(b);

      if(signA == signB)
      {
        return softfloat_subMagsF64(a, b, signA);
      }
      else
      {
        return softfloat_addMagsF64(a, b, signA);
      }
    }

    static uint64_t f64_mul(const uint64_t a, const uint64_t b)
    {
      const bool     signA = detail::signF64UI(a);
      const int16_t  expA  = detail::expF64UI (a);
            uint64_t sigA  = detail::fracF64UI(a);

      const bool     signB = detail::signF64UI(b);
      const int16_t  expB  = detail::expF64UI (b);
            uint64_t sigB  = detail::fracF64UI(b);

      const bool signZ = signA ^ signB;

      if(((!expA) && (!sigA)) || ((!expB) && (!sigB)))
      {
        return detail::packToF64UI(signZ, 0, 0);
      }
      else
      {
        int16_t expZ = (int16_t) (expA + expB) - 0x3FF;

        sigA = (sigA | UINT64_C(0x0010000000000000)) << 10U;
        sigB = (sigB | UINT64_C(0x0010000000000000)) << 11U;


        // Compute the 128-bit product of sigA and sigB.

        const uint32_t a32 = (uint32_t) (sigA >> 32U);
        const uint32_t a0  = (uint32_t)  sigA;
        const uint32_t b32 = (uint32_t) (sigB >> 32U);
        const uint32_t b0  = (uint32_t)  sigB;

        struct detail::uint128_as_struct sig128Z;

        sig128Z.v0 = ((uint64_t) a0) * b0;

        uint64_t mid1 =                    ((uint64_t) a32) * b0;
        uint64_t mid  = mid1 + (uint64_t) (((uint64_t) b32) * a0);

        sig128Z.v64  = ((uint64_t) a32) * b32;
        sig128Z.v64 += ((uint64_t) (mid < mid1) << 32U) | (uint32_t) (mid >> 32U);

        mid <<= 32U;

        sig128Z.v0  += mid;
        sig128Z.v64 += (sig128Z.v0 < mid);

        if(sig128Z.v0 != 0U)
        {
          sig128Z.v64 |= 1U;
        }

        if(sig128Z.v64 < UINT64_C(0x4000000000000000))
        {
          --expZ;

          sig128Z.v64 <<= 1U;
        }

        return softfloat_roundPackToF64(signZ, expZ, sig128Z.v64);
      }
    }

    static uint64_t f64_div(const uint64_t a, const uint64_t b)
    {
      const bool     signA = detail::signF64UI(a);
            int16_t  expA  = detail::expF64UI (a);
            uint64_t sigA  = detail::fracF64UI(a);

      const bool     signB = detail::signF64UI(b);
            int16_t  expB  = detail::expF64UI (b);
            uint64_t sigB  = detail::fracF64UI(b);

      const bool signZ = signA ^ signB;

      if((!expA) && (!sigA))
      {
        return detail::packToF64UI(signZ, 0, 0);
      }
      else
      {
        int16_t expZ = (expA - expB) + 0x3FE;

        sigA |= UINT64_C(0x0010000000000000);
        sigB |= UINT64_C(0x0010000000000000);

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

        const uint32_t recip32 = detail::softfloat_approxRecip32_1(sigB >> 32U) - 2U;
        const uint32_t sig32Z  = ((uint32_t) (sigA >> 32U) * (uint64_t) recip32) >> 32U;

        uint32_t doubleTerm = sig32Z << 1U;

        uint64_t rem =   ((sigA - (uint64_t) doubleTerm *  (uint32_t) (sigB >> 32U)) << 28U)
                                - (uint64_t) doubleTerm * ((uint32_t)  sigB >> 4U);

        uint32_t q = (((uint32_t) (rem >> 32U) * (uint64_t) recip32) >> 32U) + 4U;

        uint64_t sigZ = ((uint64_t) sig32Z << 32U) + ((uint64_t) q << 4U);

        if((sigZ & 0x1FF) < (4UL << 4U))
        {
          q    &=  (uint32_t) ~7U;
          sigZ &= ~(uint64_t) 0x7F;

          doubleTerm = q << 1U;

          rem =   ((rem - (uint64_t) doubleTerm *  (uint32_t) (sigB >> 32U)) << 28U)
                -         (uint64_t) doubleTerm * ((uint32_t)  sigB >> 4U);

          if(rem & UINT64_C(0x8000000000000000))
          {
            sigZ -= (1UL << 7U);
          }
          else
          {
            if(rem)
            {
              sigZ |= 1U;
            }
          }
        }

        return softfloat_roundPackToF64(signZ, expZ, sigZ);
      }
    }

    static uint64_t f64_sqrt(const uint64_t a)
    {
      bool     signA = detail::signF64UI(a);
      int16_t  expA  = detail::expF64UI (a);
      uint64_t sigA  = detail::fracF64UI(a);

      if(((!expA) && (!sigA)) || signA)
      {
        return a;
      }
      else
      {
        /*------------------------------------------------------------------------
        | (`sig32Z' is guaranteed to be a lower bound on the square root of
        | `sig32A', which makes `sig32Z' also a lower bound on the square root of
        | `sigA'.)
        *------------------------------------------------------------------------*/
        int16_t expZ = ((expA - 0x3FF) >> 1) + 0x3FE;

        expA &= 1;
        sigA |= UINT64_C(0x0010000000000000);

        uint32_t sig32A      = (uint32_t) (sigA >> 21U);
        uint32_t recipSqrt32 = softfloat_approxRecipSqrt32_1((uint32_t) expA, sig32A);
        uint32_t sig32Z      = ((uint64_t) sig32A * recipSqrt32) >> 32U;

        if(expA)
        {
          sigA   <<= 8U;
          sig32Z >>= 1U;
        }
        else
        {
          sigA <<= 9U;
        }

              uint64_t rem = sigA - (uint64_t) sig32Z * sig32Z;
        const uint32_t q   = ((uint32_t) (rem >> 2U) * (uint64_t) recipSqrt32) >> 32U;

        uint64_t sigZ = ((uint64_t) sig32Z << 32U | (1UL << 5U)) + ((uint64_t) q << 3U);

        if((sigZ & 0x1FFU) < 0x22U)
        {
          sigZ &= ~(uint64_t) 0x3FU;

          const uint64_t shiftedSigZ = sigZ >> 6U;

          rem = (sigA << 52U) - shiftedSigZ * shiftedSigZ;

          if(rem & UINT64_C(0x8000000000000000))
          {
            --sigZ;
          }
          else
          {
            if(rem)
            {
              sigZ |= 1U;
            }
          }
        }

        return softfloat_roundPackToF64(0, expZ, sigZ);
      }
    }

    static uint32_t f64_to_ui32(const uint64_t a)
    {
      const bool     sign = detail::signF64UI(a);
      const int16_t  exp  = detail::expF64UI (a);
            uint64_t sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      const int16_t shiftDist = 0x427 - exp;

      if(0 < shiftDist)
      {
        sig = detail::softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
      }

      return softfloat_roundToUI32(sign, sig);
    }

    static int32_t f64_to__i32(const uint64_t a)
    {
      const bool     sign = detail::signF64UI(a);
      const int16_t  exp  = detail::expF64UI (a);
            uint64_t sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int16_t shiftDist = 0x427 - exp;

      if(0 < shiftDist)
      {
        sig = detail::softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
      }

      return softfloat_roundToI32(sign, sig);
    }

    static uint64_t f64_to_ui64(const uint64_t a)
    {
      const bool     sign = detail::signF64UI(a);
      const int16_t  exp  = detail::expF64UI (a);
            uint64_t sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int16_t shiftDist = 0x433 - exp;

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
        sigExtra = detail::softfloat_shiftRightJam64Extra(sig, 0U, (uint32_t) shiftDist);
      }

      return softfloat_roundToUI64(sign, sigExtra.v);
    }

    static int64_t f64_to__i64(const uint64_t a)
    {
      const bool     sign = detail::signF64UI(a);
      const int16_t  exp  = detail::expF64UI (a);
            uint64_t sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int16_t shiftDist = 0x433 - exp;

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
        sigExtra = detail::softfloat_shiftRightJam64Extra(sig, 0U, (uint32_t) shiftDist);
      }

      return softfloat_roundToI64(sign, sigExtra.v);
    }

    static float f64_to_f32(const uint64_t a)
    {
      const bool     sign = detail::signF64UI(a);
      const int16_t  exp  = detail::expF64UI (a);
      const uint64_t frac = detail::fracF64UI(a);

      const uint32_t frac32 = (uint32_t) detail::softfloat_shortShiftRightJam64(frac, 22);

      const float f = softfloat_roundPackToF32( sign, exp - 0x381, frac32 | 0x40000000 );

      return f;
    }

    static float softfloat_roundPackToF32(bool sign, int16_t exp, uint32_t sig)
    {
      sig = (uint32_t) (sig + 0x40U) >> 7U;

      if(sig == 0U)
      {
        exp = 0;
      }

      union
      {
        float    f;
        uint32_t u;
      } uZ;

      uZ.u = detail::packToF32UI(sign, exp, sig);

      return uZ.f;
    }

    static constexpr uint64_t my__i32_to_f64(const int32_t a)
    {
      return
        (!a) ? 0U
             : detail::packToF64UI((a < 0), 0x432 - int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(uint32_t((a < 0) ? detail::negate((uint32_t) a) : (uint32_t) a)) + 21U)), (uint64_t) uint32_t((a < 0) ? detail::negate((uint32_t) a) : (uint32_t) a) << int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(uint32_t((a < 0) ? detail::negate((uint32_t) a) : (uint32_t) a)) + 21U)));
    }

    static constexpr uint64_t my__i64_to_f64(const int64_t a)
    {
      return
        (!((uint64_t) a & UINT64_C(0x7FFFFFFFFFFFFFFF)))
          ? ((a < 0) ? detail::packToF64UI(1, 0x43E, 0) : 0U)
          : softfloat_normRoundPackToF64((a < 0), 0x43C, uint64_t((a < 0) ? detail::negate((uint64_t) a) : (uint64_t) a));
    }

    static constexpr uint64_t my_ui32_to_f64(const uint32_t a)
    {
      return
        ((a == 0U)
          ? 0U
          : detail::packToF64UI(0, 0x432 - int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(a) + 21U)), ((uint64_t) a) << int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(a) + 21U))));
    }

    static constexpr uint64_t my_ui64_to_f64(const uint64_t a)
    {
      return (!a) ? (uint64_t) 0U
                  : (a & UINT64_C(0x8000000000000000)) ? softfloat_roundPackToF64(0, 0x43D, detail::softfloat_shortShiftRightJam64(a, 1))
                                                       : softfloat_normRoundPackToF64(0, 0x43C, a);
    }

    static int32_t softfloat_roundToI32(bool sign, uint64_t sig)
    {
      uint32_t sig32;

      uint16_t roundIncrement = (sign ? 0xFFFU : 0U);

      sig += roundIncrement;

      sig32 = (uint32_t) (sig >> 12U);

      const uint32_t ui = (uint32_t) (sign ? detail::negate(sig32) : sig32);

      return (int32_t) ui;
    }

    static int64_t softfloat_roundToI64(bool sign, uint64_t sig)
    {
      if(sign)
      {
        ++sig;
      }

      const uint64_t ui = (uint64_t) (sign ? detail::negate(sig) : sig);

      return (int64_t) ui;
    }

    static uint32_t softfloat_roundToUI32(bool sign, uint64_t sig)
    {
      uint16_t roundIncrement = 0U;

      if(sign)
      {
        if(!sig)
        {
          return 0U;
        }
      }

      sig += roundIncrement;

      const uint32_t z = (uint32_t) (sig >> 12);

      return z;
    }

    static uint64_t softfloat_roundToUI64(bool sign, uint64_t sig)
    {
      if(sign)
      {
        if(sig == 0U)
        {
          return 0U;
        }
      }

      return sig;
    }

    static uint64_t softfloat_addMagsF64(uint64_t uiA, uint64_t uiB, bool signZ)
    {
      const int16_t  expA = detail::expF64UI (uiA);
            uint64_t sigA = detail::fracF64UI(uiA);
      const int16_t  expB = detail::expF64UI (uiB);
            uint64_t sigB = detail::fracF64UI(uiB);

      const int16_t expDiff = expA - expB;

      int16_t  expZ;
      uint64_t sigZ;

      if(!expDiff)
      {
        expZ = expA;
        sigZ = (uint64_t) (UINT64_C(0x0020000000000000) + sigA) + sigB;
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

          sigA = detail::softfloat_shiftRightJam64(sigA, (uint32_t) (-expDiff));
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

          sigB = detail::softfloat_shiftRightJam64(sigB, (uint32_t) expDiff);
        }

        sigZ = (uint64_t) (UINT64_C(0x2000000000000000) + sigA) + sigB;

        if(sigZ < UINT64_C(0x4000000000000000))
        {
          --expZ;

          sigZ <<= 1U;
        }
      }

      return softfloat_roundPackToF64(signZ, expZ, sigZ);
    }

    /*----------------------------------------------------------------------------
    | Returns an approximation to the reciprocal of the square root of the number
    | represented by 'a', where 'a' is interpreted as an unsigned fixed-point
    | number either with one integer bit and 31 fraction bits or with two integer
    | bits and 30 fraction bits.
    *----------------------------------------------------------------------------*/
    static uint32_t softfloat_approxRecipSqrt32_1(uint32_t oddExpA, uint32_t a)
    {
      constexpr std::array<uint16_t, 16U> softfloat_approxRecipSqrt_1k0s =
      {{
        UINT16_C(0xB4C9), UINT16_C(0xFFAB), UINT16_C(0xAA7D), UINT16_C(0xF11C),
        UINT16_C(0xA1C5), UINT16_C(0xE4C7), UINT16_C(0x9A43), UINT16_C(0xDA29),
        UINT16_C(0x93B5), UINT16_C(0xD0E5), UINT16_C(0x8DED), UINT16_C(0xC8B7),
        UINT16_C(0x88C6), UINT16_C(0xC16D), UINT16_C(0x8424), UINT16_C(0xBAE1)
      }};

      constexpr std::array<uint16_t, 16U> softfloat_approxRecipSqrt_1k1s =
      {{
        UINT16_C(0xA5A5), UINT16_C(0xEA42), UINT16_C(0x8C21), UINT16_C(0xC62D),
        UINT16_C(0x788F), UINT16_C(0xAA7F), UINT16_C(0x6928), UINT16_C(0x94B6),
        UINT16_C(0x5CC7), UINT16_C(0x8335), UINT16_C(0x52A6), UINT16_C(0x74E2),
        UINT16_C(0x4A3E), UINT16_C(0x68FE), UINT16_C(0x432B), UINT16_C(0x5EFD)
      }};

      int16_t index       = (int16_t) (((uint32_t) (a >> 27U) & 0xEU) + oddExpA);
      uint16_t     eps    = (uint16_t) (a >> 12);
      uint16_t     r0     =                           softfloat_approxRecipSqrt_1k0s[(unsigned) index]
                            - (uint16_t) ((uint32_t) (softfloat_approxRecipSqrt_1k1s[(unsigned) index] * (uint32_t) eps) >> 20U);
      uint32_t     ESqrR0 = (uint32_t) r0 * r0;

      if(!oddExpA)
      {
        ESqrR0 <<= 1U;
      }

      const uint32_t sigma0 = ~(uint32_t) (((uint32_t) ESqrR0 * (uint64_t) a) >> 23U);

      uint32_t r = (((uint32_t) r0) << 16U) + (uint32_t) ((r0 * (uint64_t) sigma0) >> 25U);

      const uint32_t sqrSigma0 = (uint32_t) ((uint64_t) ((uint64_t) sigma0 * sigma0) >> 32U);

      r += (uint32_t) ((uint64_t) ((uint32_t) ((uint32_t) ((r >> 1U) + (r >> 3U)) - ((uint32_t) r0 << 14U)) * (uint64_t) sqrSigma0) >> 48U);

      if(!(r & UINT32_C(0x80000000)))
      {
        r = UINT32_C(0x80000000);
      }

      return r;
    }

    static uint64_t softfloat_normRoundPackToF64(bool sign, int16_t exp, uint64_t sig)
    {
      const int_fast8_t shiftDist = (int_fast8_t) (detail::softfloat_countLeadingZeros64(sig) - 1U);

      exp -= shiftDist;

      if((10 <= shiftDist) && ((uint32_t) exp < 0x7FDU))
      {
        const uint64_t uZ = detail::packToF64UI(sign, sig ? exp : 0, sig << (shiftDist - 10));

        return uZ;
      }
      else
      {
        return softfloat_roundPackToF64(sign, exp, sig << shiftDist);
      }
    }

    static uint64_t softfloat_roundPackToF64(bool sign, int16_t exp, uint64_t sig)
    {
      uint16_t roundIncrement = 0x200U;

      uint16_t roundBits = sig & 0x3FFU;

      if(0x7FDU <= (uint16_t) exp)
      {
        if(exp < 0)
        {
          sig = detail::softfloat_shiftRightJam64(sig, (uint32_t) -exp);
          exp = 0;
          roundBits = sig & 0x3FFU;
        }
      }

      sig = (sig + roundIncrement) >> 10U;

      sig &= (uint64_t) (~(uint64_t) (((roundBits ^ 0x200U) == 0U ? 1U : 0U) & 1U));

      if(!sig)
      {
        exp = 0;
      }

      const uint64_t uiZ = detail::packToF64UI(sign, exp, sig);

      return uiZ;
    }

    static uint64_t softfloat_subMagsF64(uint64_t uiA, uint64_t uiB, bool signZ)
    {
      uint64_t uiZ;
      int16_t  expZ;

      int16_t  expA = detail::expF64UI(uiA);
      uint64_t sigA = detail::fracF64UI(uiA);
      int16_t  expB = detail::expF64UI(uiB);
      uint64_t sigB = detail::fracF64UI(uiB);

      const int16_t expDiff = expA - expB;

      if(!expDiff)
      {
        int64_t sigDiff = (int64_t) (sigA - sigB);

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

          int_fast8_t shiftDist = (int_fast8_t) (detail::softfloat_countLeadingZeros64((uint64_t) sigDiff) - 11U);

          expZ = expA - shiftDist;

          if(expZ < 0)
          {
            shiftDist = (int_fast8_t) expA;

            expZ = 0;
          }

          uiZ = detail::packToF64UI(signZ, expZ, sigDiff << shiftDist);
        }
      }
      else
      {
        sigA <<= 10U;
        sigB <<= 10U;

        uint64_t sigZ;

        if(expDiff < 0)
        {
          signZ = (!signZ);

          sigA += ((expA != 0) ? UINT64_C(0x4000000000000000) : sigA);
          sigA  = detail::softfloat_shiftRightJam64(sigA, (uint32_t) -expDiff);

          sigB |= UINT64_C(0x4000000000000000);

          expZ = expB;
          sigZ = sigB - sigA;
        }
        else
        {
          sigB += ((expB != 0) ? UINT64_C(0x4000000000000000) : sigB);
          sigB  = detail::softfloat_shiftRightJam64(sigB, (uint32_t) expDiff);

          sigA |= UINT64_C(0x4000000000000000);

          expZ = expA;
          sigZ = sigA - sigB;
        }

        return softfloat_normRoundPackToF64(signZ, (int16_t) (expZ - 1), sigZ);
      }

      return uiZ;
    }

    friend bool isfinite(const soft_double x) { return ((isnan(x) == false) && (isinf(x) == false)); }
    friend bool isnan   (const soft_double x) { return  (x.my_value == my_value_quiet_NaN().my_value); }
    friend bool isinf   (const soft_double x) { return ((x.my_value & my_value_infinity().my_value) == my_value_infinity().my_value); }

    friend inline soft_double fabs (const soft_double x) { return soft_double((uint64_t) (x.my_value & UINT64_C(0x7FFFFFFFFFFFFFFF)), detail::nothing()); }
    friend inline soft_double sqrt (const soft_double x) { return soft_double(f64_sqrt(x.my_value), detail::nothing()); }

    friend inline soft_double operator+(const soft_double& a, const soft_double& b) { return soft_double(f64_add(a.my_value, b.my_value), detail::nothing()); }
    friend inline soft_double operator-(const soft_double& a, const soft_double& b) { return soft_double(f64_sub(a.my_value, b.my_value), detail::nothing()); }
    friend inline soft_double operator*(const soft_double& a, const soft_double& b) { return soft_double(f64_mul(a.my_value, b.my_value), detail::nothing()); }
    friend inline soft_double operator/(const soft_double& a, const soft_double& b) { return soft_double(f64_div(a.my_value, b.my_value), detail::nothing()); }

    // Global add/sub/mul/div of const decwide_t<MyDigits10, LimbType, AllocatorType, InternalFloatType>& with all built-in types.
    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator+(const soft_double& u, UnsignedIntegralType n)
    {
      return soft_double(u) += soft_double(n);
    }

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator-(const soft_double& u, UnsignedIntegralType n)
    {
      return soft_double(u) -= soft_double(n);
    }

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator*(const soft_double& u, UnsignedIntegralType n)
    {
      return soft_double(u) *= soft_double(n);
    }

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator/(const soft_double& u, UnsignedIntegralType n)
    {
      return soft_double(u) /= soft_double(n);
    }

    friend inline soft_double operator+(const soft_double& u, float f) { return soft_double(u) += soft_double(f); }
    friend inline soft_double operator-(const soft_double& u, float f) { return soft_double(u) -= soft_double(f); }
    friend inline soft_double operator*(const soft_double& u, float f) { return soft_double(u) *= soft_double(f); }
    friend inline soft_double operator/(const soft_double& u, float f) { return soft_double(u) /= soft_double(f); }

    friend inline soft_double operator+(float f, const soft_double& u) { return soft_double(f) += u; }
    friend inline soft_double operator-(float f, const soft_double& u) { return soft_double(f) -= u; }
    friend inline soft_double operator*(float f, const soft_double& u) { return soft_double(f) *= u; }
    friend inline soft_double operator/(float f, const soft_double& u) { return soft_double(f) /= u; }

    friend inline soft_double operator+(const soft_double&, double) = delete;
    friend inline soft_double operator-(const soft_double&, double) = delete;
    friend inline soft_double operator*(const soft_double&, double) = delete;
    friend inline soft_double operator/(const soft_double&, double) = delete;

    friend inline soft_double operator+(double, const soft_double&) = delete;
    friend inline soft_double operator-(double, const soft_double&) = delete;
    friend inline soft_double operator*(double, const soft_double&) = delete;
    friend inline soft_double operator/(double, const soft_double&) = delete;

    friend inline soft_double operator+(const soft_double& u, long double) = delete;
    friend inline soft_double operator-(const soft_double& u, long double) = delete;
    friend inline soft_double operator*(const soft_double& u, long double) = delete;
    friend inline soft_double operator/(const soft_double& u, long double) = delete;

    friend inline soft_double operator+(long double, const soft_double&) = delete;
    friend inline soft_double operator-(long double, const soft_double&) = delete;
    friend inline soft_double operator*(long double, const soft_double&) = delete;
    friend inline soft_double operator/(long double, const soft_double&) = delete;

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator+(UnsignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) += u;
    }

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator-(UnsignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) -= u;
    }

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator*(UnsignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) *= u;
    }

    template<typename UnsignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <UnsignedIntegralType>::value == true)
                                          && (std::is_unsigned   <UnsignedIntegralType>::value == true), soft_double>::type
    operator/(UnsignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) /= u;
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator+(const soft_double& u, SignedIntegralType n)
    {
      return soft_double(u) += soft_double(n);
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator-(const soft_double& u, SignedIntegralType n)
    {
      return soft_double(u) -= soft_double(n);
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator*(const soft_double& u, SignedIntegralType n)
    {
      return soft_double(u) *= soft_double(n);
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator/(const soft_double& u, SignedIntegralType n)
    {
      return soft_double(u) /= soft_double(n);
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator+(SignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) += u;
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator-(SignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) -= u;
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator*(SignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) *= u;
    }

    template<typename SignedIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <SignedIntegralType>::value == true)
                                          && (std::is_unsigned   <SignedIntegralType>::value == false), soft_double>::type
    operator/(SignedIntegralType n, const soft_double& u)
    {
      return soft_double(n) /= u;
    }

    friend inline constexpr bool operator< (const soft_double& a, const soft_double& b) { return my_lt(a, b); }
    friend inline constexpr bool operator<=(const soft_double& a, const soft_double& b) { return my_le(a, b); }
    friend inline constexpr bool operator==(const soft_double& a, const soft_double& b) { return  (a.my_value == b.my_value); }
    friend inline constexpr bool operator!=(const soft_double& a, const soft_double& b) { return ((a == b) == false); }
    friend inline constexpr bool operator>=(const soft_double& a, const soft_double& b) { return ((a <  b) == false); }
    friend inline constexpr bool operator> (const soft_double& a, const soft_double& b) { return ((a <= b) == false); }
  };

  using float64_t = soft_double;
  } } // namespace math::softfloat

  namespace std {

  // Specialization of numeric_limits for soft_double.
  template<>
  class numeric_limits<math::softfloat::soft_double>
  {
  public:
    static constexpr bool               is_specialized    = true;
    static constexpr float_denorm_style has_denorm        = denorm_present;
    static constexpr bool               has_infinity      = true;
    static constexpr bool               has_quiet_NaN     = true;
    static constexpr bool               has_signaling_NaN = false;
    static constexpr bool               is_bounded        = true;
    static constexpr bool               is_iec559         = false;
    static constexpr bool               is_signed         = true;
    static constexpr float_round_style  round_style       = round_to_nearest;
    static constexpr int                radix             = 2;

    static constexpr int                digits            = 53;
    static constexpr int                digits10          = 15;
    static constexpr int                max_digits10      = 17;
    static constexpr int                max_exponent      = 1024;
    static constexpr int                max_exponent10    = 308;
    static constexpr int                min_exponent      = -1021;
    static constexpr int                min_exponent10    = -307;

    static constexpr math::softfloat::soft_double (min)        () noexcept { return math::softfloat::soft_double::my_value_min(); }
    static constexpr math::softfloat::soft_double (max)        () noexcept { return math::softfloat::soft_double::my_value_max(); }
    static constexpr math::softfloat::soft_double lowest       () noexcept { return math::softfloat::soft_double::my_value_lowest(); }
    static constexpr math::softfloat::soft_double epsilon      () noexcept { return math::softfloat::soft_double::my_value_epsilon(); }
    static constexpr math::softfloat::soft_double round_error  () noexcept { return math::softfloat::soft_double::my_value_round_error(); }
    static constexpr math::softfloat::soft_double denorm_min   () noexcept { return math::softfloat::soft_double::my_value_denorm_min(); }
    static constexpr math::softfloat::soft_double infinity     () noexcept { return math::softfloat::soft_double::my_value_infinity(); }
    static constexpr math::softfloat::soft_double quiet_NaN    () noexcept { return math::softfloat::soft_double::my_value_quiet_NaN(); }
    static constexpr math::softfloat::soft_double signaling_NaN() noexcept { return math::softfloat::soft_double::my_value_signaling_NaN(); }
  };

  }

#endif // SOFT_DOUBLE_2020_10_27_H_
