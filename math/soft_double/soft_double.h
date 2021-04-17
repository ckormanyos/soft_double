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

  namespace math { namespace sd {

  namespace detail {

  struct uint128_as_struct  { uint64_t v0, v64; };
  struct uint64_extra       { uint64_t extra, v; };

  constexpr bool         signF32UI(uint32_t a) { return ((bool) ((uint32_t) (a)>>31)); }
  constexpr int_fast16_t expF32UI (uint32_t a) { return ((int_fast16_t) ((a)>>23) & 0xFF); }
  constexpr uint32_t     fracF32UI(uint32_t a) { return ((a) & UINT32_C(0x007FFFFF)); }

  constexpr bool         signF64UI(uint64_t a) { return ((bool) ((uint64_t) (a)>>63)); }
  constexpr int_fast16_t expF64UI (uint64_t a) { return ((int_fast16_t) ((a)>>52) & 0x7FF); }
  constexpr uint64_t     fracF64UI(uint64_t a) { return ((a) & UINT64_C(0x000FFFFFFFFFFFFF)); }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr uint64_t packToF64UI(bool sign, IntegralTypeExp exp, IntegralTypeSig sig)
  {
    return ((uint64_t) ((((uint64_t) (sign ? 1ULL : 0ULL))<<63) + (((uint64_t) (exp))<<52) + (uint64_t) (sig)));
  }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr uint32_t packToF32UI(bool sign, IntegralTypeExp exp, IntegralTypeSig sig)
  {
    return ((uint32_t) ((((uint32_t) (sign ? 1ULL : 0ULL))<<31) + (((uint32_t) (exp))<<23) + (uint32_t) (sig)));
  }

  /*----------------------------------------------------------------------------
  | Shifts 'a' right by the number of bits given in 'dist', which must be in
  | the range 1 to 63.  If any nonzero bits are shifted off, they are "jammed"
  | into the least-significant bit of the shifted value by setting the least-
  | significant bit to 1.  This shifted-and-jammed value is returned.
  *----------------------------------------------------------------------------*/
  constexpr uint64_t softfloat_shortShiftRightJam64(uint64_t a, uint_fast8_t dist)
  {
    return a >> dist | ((a & (((uint64_t) 1 << dist) - 1)) != 0);
  }

  /*----------------------------------------------------------------------------
  | Shifts 'a' right by the number of bits given in 'dist', which must not
  | be zero.  If any nonzero bits are shifted off, they are "jammed" into the
  | least-significant bit of the shifted value by setting the least-significant
  | bit to 1.  This shifted-and-jammed value is returned.
  |   The value of 'dist' can be arbitrarily large.  In particular, if 'dist' is
  | greater than 32, the result will be either 0 or 1, depending on whether 'a'
  | is zero or nonzero.
  *----------------------------------------------------------------------------*/
  constexpr uint32_t softfloat_shiftRightJam32(uint32_t a, uint_fast16_t dist)
  {
    return (dist < 31) ? a >> dist | ((uint32_t)(a << (-dist & 31)) != 0) : (a != 0);
  }

  /*----------------------------------------------------------------------------
  | Shifts 'a' right by the number of bits given in 'dist', which must not
  | be zero.  If any nonzero bits are shifted off, they are "jammed" into the
  | least-significant bit of the shifted value by setting the least-significant
  | bit to 1.  This shifted-and-jammed value is returned.
  |   The value of 'dist' can be arbitrarily large.  In particular, if 'dist' is
  | greater than 64, the result will be either 0 or 1, depending on whether 'a'
  | is zero or nonzero.
  *----------------------------------------------------------------------------*/
  constexpr uint64_t softfloat_shiftRightJam64(uint64_t a, uint32_t dist)
  {
    return (dist < 63) ? a >> dist | ((uint64_t)(a << (-dist & 63)) != 0) : (a != 0);
  }

  /*----------------------------------------------------------------------------
  | A constant table that translates an 8-bit unsigned integer (the array index)
  | into the number of leading 0 bits before the most-significant 1 of that
  | integer.  For integer zero (index 0), the corresponding table element is 8.
  *----------------------------------------------------------------------------*/
  constexpr uint_least8_t softfloat_countLeadingZeros8_z_table(const uint_fast8_t index)
  {
    return ((index < 0x1U) ? 4U :
           ((index < 0x2U) ? 3U :
           ((index < 0x4U) ? 2U :
           ((index < 0x8U) ? 1U : 0U))));
  }

  constexpr uint_least8_t softfloat_countLeadingZeros8(const uint_fast8_t index)
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
    uint_fast8_t count = 0;

    if(a < UINT32_C(0x10000))
    {
      count = 16U;
      a   <<= 16U;
    }

    if(a < UINT32_C(0x1000000))
    {
      count += 8U;
      a    <<= 8U;
    }

    count += softfloat_countLeadingZeros8((uint_fast8_t) (a >> 24U));

    return count;
  }

  /*----------------------------------------------------------------------------
  | Returns the number of leading 0 bits before the most-significant 1 bit of
  | 'a'.  If 'a' is zero, 64 is returned.
  *----------------------------------------------------------------------------*/
  constexpr uint_fast8_t softfloat_countLeadingZeros64(uint64_t a)
  {
    uint_fast8_t count = 0U;

    uint32_t a32   = (uint32_t) (a >> 32);

    if(!a32)
    {
      count = 32U;
      a32 = (uint32_t) a;
    }

    /*------------------------------------------------------------------------
    | From here, result is current count + count leading zeros of `a32'.
    *------------------------------------------------------------------------*/
    if(a32 < UINT32_C(0x10000))
    {
      count += 16U;
      a32  <<= 16U;
    }

    if(a32 < UINT32_C(0x1000000))
    {
      count += 8U;
      a32  <<= 8U;
    }

    count += softfloat_countLeadingZeros8(a32 >> 24U);

    return count;
  }

  /*----------------------------------------------------------------------------
  | Returns an approximation to the reciprocal of the number represented by 'a',
  | where 'a' is interpreted as an unsigned fixed-point number with one integer
  | bit and 31 fraction bits.  The 'a' input must be "normalized", meaning that
  | its most-significant bit (bit 31) must be 1.  Thus, if A is the value of
  | the fixed-point interpretation of 'a', then 1 <= A < 2.  The returned value
  | is interpreted as a pure unsigned fraction, having no integer bits and 32
  | fraction bits.  The approximation returned is never greater than the true
  | reciprocal 1/A, and it differs from the true reciprocal by at most 2.006 ulp
  | (units in the last place).
  *----------------------------------------------------------------------------*/
  constexpr uint32_t softfloat_approxRecip32_1(uint32_t a)
  {
    return (uint32_t) (UINT64_C( 0x7FFFFFFFFFFFFFFF ) / a);
  }

  /*----------------------------------------------------------------------------
  | Shifts the 128 bits formed by concatenating 'a' and 'extra' right by 64
  | _plus_ the number of bits given in 'dist', which must not be zero.  This
  | shifted value is at most 64 nonzero bits and is returned in the 'v' field
  | of the 'struct uint64_extra' result.  The 64-bit 'extra' field of the result
  | contains a value formed as follows from the bits that were shifted off:  The
  | _last_ bit shifted off is the most-significant bit of the 'extra' field, and
  | the other 63 bits of the 'extra' field are all zero if and only if _all_but_
  | _the_last_ bits shifted off were all zero.
  |   (This function makes more sense if 'a' and 'extra' are considered to form
  | an unsigned fixed-point number with binary point between 'a' and 'extra'.
  | This fixed-point value is shifted right by the number of bits given in
  | 'dist', and the integer part of this shifted value is returned in the 'v'
  | field of the result.  The fractional part of the shifted value is modified
  | as described above and returned in the 'extra' field of the result.)
  *----------------------------------------------------------------------------*/
  constexpr struct uint64_extra softfloat_shiftRightJam64Extra(uint64_t a, uint64_t extra, uint32_t dist)
  {
    return
    {
      (dist < 64) ? (a << (-dist & 63)) | (extra != 0 ? 1 : 0) : ((dist == 64) ? a : (a != 0 ? 1 : 0))  | (extra != 0 ? 1 : 0),
      (dist < 64) ?  a >> dist                                 : 0
    };
  }

  struct nothing { };

  }

  class soft_double;

  inline soft_double operator+(const soft_double& a, const soft_double& b);
  inline soft_double operator-(const soft_double& a, const soft_double& b);
  inline soft_double operator*(const soft_double& a, const soft_double& b);
  inline soft_double operator/(const soft_double& a, const soft_double& b);

  template<typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
  operator+(const soft_double& u, BuiltinIntegralType n);

  template<typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
  operator-(const soft_double& u, BuiltinIntegralType n);

  template<typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
  operator*(const soft_double& u, BuiltinIntegralType n);

  template<typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
  operator/(const soft_double& u, BuiltinIntegralType n);

  inline bool operator< (const soft_double& a, const soft_double& b);
  inline bool operator<=(const soft_double& a, const soft_double& b);
  inline bool operator==(const soft_double& a, const soft_double& b);
  inline bool operator!=(const soft_double& a, const soft_double& b);
  inline bool operator>=(const soft_double& a, const soft_double& b);
  inline bool operator> (const soft_double& a, const soft_double& b);

  inline bool        isnan   (const soft_double x);
  inline bool        isinf   (const soft_double x);
  inline bool        isfinite(const soft_double x);
  inline soft_double fabs    (const soft_double x);
  inline soft_double sqrt    (const soft_double x);

  class soft_double final
  {
  public:
    using representation_type = uint64_t;

    soft_double() { }

    constexpr soft_double(const std::int8_t  n) : my_value(my__i32_to_f64((std::int32_t) n)) { }
    constexpr soft_double(const std::int16_t n) : my_value(my__i32_to_f64((std::int32_t) n)) { }
    constexpr soft_double(const std::int32_t n) : my_value(my__i32_to_f64(               n)) { }
    constexpr soft_double(const std::int64_t n) : my_value(my__i64_to_f64(               n)) { }

    constexpr soft_double(const std::uint8_t  u) : my_value(my_ui32_to_f64((std::uint32_t) u)) { }
    constexpr soft_double(const std::uint16_t u) : my_value(my_ui32_to_f64((std::uint32_t) u)) { }
    constexpr soft_double(const std::uint32_t u) : my_value(my_ui32_to_f64(                u)) { }
    constexpr soft_double(const std::uint64_t u) : my_value(my_ui64_to_f64(                u)) { }

    static_assert(sizeof(float) == 4U,
                  "Error: This template is designed for 4 byte built-in float");

    soft_double(const float f) : my_value()
    {
      const uint32_t uiA = *((volatile uint32_t*) &f);

      const bool         sign = detail::signF32UI(uiA);
      const int_fast16_t exp  = detail::expF32UI(uiA);
      const uint32_t     frac = detail::fracF32UI(uiA);

      if((!exp) && (!frac))
      {
        my_value = detail::packToF64UI(sign, 0, 0);
      }
      else
      {
        my_value = detail::packToF64UI(sign, exp + 0x380, (uint64_t) frac << 29);
      }
    }

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

    const soft_double& operator+() const { return *this; }
          soft_double  operator-() const { return soft_double(my_value ^ (uint64_t) (1ULL << 63U), detail::nothing()); }

    static constexpr representation_type get_value(soft_double x) { return x.my_value; }

    static constexpr soft_double my_value_zero() { return soft_double(UINT64_C(0),                   detail::nothing()); }
    static constexpr soft_double my_value_one () { return soft_double(UINT64_C(1),                   detail::nothing()); }
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

    bool my_le(const soft_double& b) const
    {
      const bool signA = detail::signF64UI(  my_value);
      const bool signB = detail::signF64UI(b.my_value);

      return (signA != signB) ? (signA || !((my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF)))
                              : (my_value == b.my_value) || (signA ^ (my_value < b.my_value));
    }

    bool my_lt(const soft_double& b) const
    {
      const bool signA = detail::signF64UI(  my_value);
      const bool signB = detail::signF64UI(b.my_value);

      return (signA != signB) ? signA && ((my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF))
                              : (my_value != b.my_value) && (signA ^ (my_value < b.my_value));
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
      const bool         signA = detail::signF64UI(a);
      const int_fast16_t expA  = detail::expF64UI (a);
            uint64_t     sigA  = detail::fracF64UI(a);

      const bool         signB = detail::signF64UI(b);
      const int_fast16_t expB  = detail::expF64UI (b);
            uint64_t     sigB  = detail::fracF64UI(b);

      const bool signZ = signA ^ signB;

      if(((!expA) && (!sigA)) || ((!expB) && (!sigB)))
      {
        return detail::packToF64UI(signZ, 0, 0);
      }
      else
      {
        int_fast16_t expZ = (int_fast16_t) (expA + expB) - 0x3FF;

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
      const bool         signA = detail::signF64UI(a);
            int_fast16_t expA  = detail::expF64UI (a);
            uint64_t     sigA  = detail::fracF64UI(a);

      const bool         signB = detail::signF64UI(b);
            int_fast16_t expB  = detail::expF64UI (b);
            uint64_t     sigB  = detail::fracF64UI(b);

      const bool signZ = signA ^ signB;

      if((!expA) && (!sigA))
      {
        return detail::packToF64UI(signZ, 0, 0);
      }
      else
      {
        int_fast16_t expZ = (expA - expB) + 0x3FE;

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

        const uint32_t recip32    = detail::softfloat_approxRecip32_1(sigB >> 32U) - 2U;
        const uint32_t sig32Z     = ((uint32_t) (sigA >> 32U) * (uint64_t) recip32) >> 32U;

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
      bool         signA = detail::signF64UI(a);
      int_fast16_t expA  = detail::expF64UI (a);
      uint64_t     sigA  = detail::fracF64UI(a);

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
        int_fast16_t expZ = ((expA - 0x3FF) >> 1) + 0x3FE;
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
      bool          sign = detail::signF64UI(a);
      int_fast16_t  exp  = detail::expF64UI (a);
      uint64_t      sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x427 - exp;

      if(0 < shiftDist)
      {
        sig = detail::softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
      }

      return softfloat_roundToUI32(sign, sig);
    }

    static int32_t f64_to__i32(const uint64_t a)
    {
      bool          sign = detail::signF64UI(a);
      int_fast16_t  exp  = detail::expF64UI (a);
      uint64_t      sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x427 - exp;

      if(0 < shiftDist)
      {
        sig = detail::softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
      }

      return softfloat_roundToI32(sign, sig);
    }

    static uint64_t f64_to_ui64(const uint64_t a)
    {
      const bool          sign = detail::signF64UI(a);
      const int_fast16_t  exp  = detail::expF64UI (a);
            uint64_t      sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x433 - exp;

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
      const bool          sign = detail::signF64UI(a);
      const int_fast16_t  exp  = detail::expF64UI (a);
            uint64_t      sig  = detail::fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x433 - exp;

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
      const bool          sign = detail::signF64UI(a);
      const int_fast16_t  exp  = detail::expF64UI (a);
      const uint64_t      frac = detail::fracF64UI(a);

      const uint32_t frac32 = (uint32_t) detail::softfloat_shortShiftRightJam64( frac, 22 );

      const float f = softfloat_roundPackToF32( sign, exp - 0x381, frac32 | 0x40000000 );

      return f;
    }

    static float softfloat_roundPackToF32( bool sign, int_fast16_t exp, uint_fast32_t sig )
    {
      constexpr uint_fast8_t roundIncrement = UINT8_C(0x40);

      const uint_fast8_t roundBits = sig & 0x7F;

      sig = (sig + roundIncrement)>>7;

      sig &= ~(uint_fast32_t) (! (roundBits ^ 0x40) & 0);

      if(!sig)
      {
        exp = 0;
      }

      const uint_fast32_t u = detail::packToF32UI(sign, exp, sig);

      const float f = *(volatile float*) &u;

      return f;
    }

    static constexpr uint64_t my__i32_to_f64(const int32_t a)
    {
      return
        (!a) ? 0U
             : detail::packToF64UI((a < 0), 0x432 - int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a)) + 21U)), (uint64_t) uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a) << int_fast8_t((int_fast8_t) (detail::softfloat_countLeadingZeros32(uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a)) + 21U)));
    }

    static constexpr uint64_t my__i64_to_f64(const int64_t a)
    {
      return
        (!((uint64_t) a & UINT64_C(0x7FFFFFFFFFFFFFFF)))
          ? ((a < 0) ? detail::packToF64UI(1, 0x43E, 0) : 0U)
          : softfloat_normRoundPackToF64((a < 0), 0x43C, uint64_t((a < 0) ? -(uint64_t) a : (uint64_t) a));
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

      uint_fast16_t roundIncrement = (sign ? 0xFFF : 0);

      sig += roundIncrement;

      sig32 = (uint32_t) (sig >> 12);

      union
      {
        uint32_t ui;
         int32_t  i;
      } uZ;

      uZ.ui = sign ? -sig32 : sig32;

      return uZ.i;
    }

    static int64_t softfloat_roundToI64(bool sign, uint64_t sig)
    {
      if(sign)
      {
        ++sig;
      }

      union
      {
        uint64_t ui;
        int64_t i;
      } uZ;

      uZ.ui = sign ? -sig : sig;

      return uZ.i;
    }

    static uint32_t softfloat_roundToUI32(bool sign, uint64_t sig)
    {
      uint_fast16_t roundIncrement = 0;

      if(sign)
      {
        if(!sig)
        {
          return 0;
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
        if(!(sig))
        {
          return 0;
        }
      }

      return sig;
    }

    static uint64_t softfloat_addMagsF64(uint64_t uiA, uint64_t uiB, bool signZ)
    {
      const int_fast16_t expA = detail::expF64UI (uiA);
            uint64_t     sigA = detail::fracF64UI(uiA);
      const int_fast16_t expB = detail::expF64UI (uiB);
            uint64_t     sigB = detail::fracF64UI(uiB);

      const int_fast16_t expDiff = expA - expB;

      int_fast16_t  expZ;
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
    | bits and 30 fraction bits.  The format of 'a' is determined by 'oddExpA',
    | which must be either 0 or 1.  If 'oddExpA' is 1, 'a' is interpreted as
    | having one integer bit, and if 'oddExpA' is 0, 'a' is interpreted as having
    | two integer bits.  The 'a' input must be "normalized", meaning that its
    | most-significant bit (bit 31) must be 1.  Thus, if A is the value of the
    | fixed-point interpretation of 'a', it follows that 1 <= A < 2 when 'oddExpA'
    | is 1, and 2 <= A < 4 when 'oddExpA' is 0.
    |   The returned value is interpreted as a pure unsigned fraction, having
    | no integer bits and 32 fraction bits.  The approximation returned is never
    | greater than the true reciprocal 1/sqrt(A), and it differs from the true
    | reciprocal by at most 2.06 ulp (units in the last place).  The approximation
    | returned is also always within the range 0.5 to 1; thus, the most-
    | significant bit of the result is always set.
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

      int_fast16_t index  = (int_fast16_t) (((uint32_t) (a >> 27U) & 0xEU) + oddExpA);
      uint16_t     eps    = (uint16_t) (a >> 12);
      uint16_t     r0     =     softfloat_approxRecipSqrt_1k0s[index]
                            - ((softfloat_approxRecipSqrt_1k1s[index] * (uint32_t) eps) >> 20U);
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

    static uint64_t softfloat_normRoundPackToF64(bool sign, int_fast16_t exp, uint64_t sig)
    {
      const int_fast8_t shiftDist = (int_fast8_t) (detail::softfloat_countLeadingZeros64(sig) - 1U);

      exp -= shiftDist;

      if((10 <= shiftDist) && ((uint32_t) exp < 0x7FD))
      {
        const uint64_t uZ = detail::packToF64UI(sign, sig ? exp : 0, sig << (shiftDist - 10));

        return uZ;
      }
      else
      {
        return softfloat_roundPackToF64(sign, exp, sig << shiftDist);
      }
    }

    static uint64_t softfloat_roundPackToF64(bool sign, int_fast16_t exp, uint64_t sig)
    {
      uint_fast16_t roundIncrement = 0x200U;

      uint_fast16_t roundBits = sig & 0x3FFU;

      if(0x7FD <= (uint16_t) exp)
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
      uint64_t     uiZ;
      int_fast16_t expZ;

      int_fast16_t expA = detail::expF64UI(uiA);
      uint64_t     sigA = detail::fracF64UI(uiA);
      int_fast16_t expB = detail::expF64UI(uiB);
      uint64_t     sigB = detail::fracF64UI(uiB);

      const int_fast16_t expDiff = expA - expB;

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

        return softfloat_normRoundPackToF64(signZ, (int_fast16_t) (expZ - 1), sigZ);
      }

      return uiZ;
    }

    friend bool isfinite(const soft_double x) { return ((isnan(x) == false) && (isinf(x) == false)); }
    friend bool isnan   (const soft_double x) { return  (x.my_value == my_value_quiet_NaN().my_value); }
    friend bool isinf   (const soft_double x) { return ((x.my_value & my_value_infinity().my_value) == my_value_infinity().my_value); }

    friend inline soft_double fabs (const soft_double x) { return soft_double((uint64_t) (x.my_value & UINT64_C(0x7FFFFFFFFFFFFFFF)), detail::nothing()); }
    friend inline soft_double sqrt (const soft_double x) { return soft_double(f64_sqrt(x.my_value), detail::nothing()); }

    friend inline soft_double operator+(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_add(a.my_value, b.my_value); return result; }
    friend inline soft_double operator-(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_sub(a.my_value, b.my_value); return result; }
    friend inline soft_double operator*(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_mul(a.my_value, b.my_value); return result; }
    friend inline soft_double operator/(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_div(a.my_value, b.my_value); return result; }

    // Global add/sub/mul/div of const decwide_t<MyDigits10, LimbType, AllocatorType, InternalFloatType>& with all built-in types.
    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator+(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) += soft_double(n);
    }

    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator-(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) -= soft_double(n);
    }

    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator*(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) *= soft_double(n);
    }

    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator/(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) /= soft_double(n);
    }

    friend inline bool operator< (const soft_double& a, const soft_double& b) { return a.my_lt(b); }
    friend inline bool operator<=(const soft_double& a, const soft_double& b) { return a.my_le(b); }
    friend inline bool operator==(const soft_double& a, const soft_double& b) { return  (a.my_value == b.my_value); }
    friend inline bool operator!=(const soft_double& a, const soft_double& b) { return ((a == b) == false); }
    friend inline bool operator>=(const soft_double& a, const soft_double& b) { return ((a <  b) == false); }
    friend inline bool operator> (const soft_double& a, const soft_double& b) { return ((a <= b) == false); }
  };

  using float64_t = soft_double;
  } } // namespace math::sd

  namespace std {

  // TBD: Do not derive directly from numeric_limits,
  // as some members such as is_iec559, etc. should be false.
  template<> class numeric_limits< ::math::sd::float64_t>
  {
  public:
    static constexpr bool               is_specialized           = true;
    static constexpr float_denorm_style has_denorm = denorm_present;
    static constexpr bool               has_infinity             = true;
    static constexpr bool               has_quiet_NaN            = true;
    static constexpr bool               has_signaling_NaN        = false;
    static constexpr bool               is_bounded               = true;
    static constexpr bool               is_iec559                = false;
    static constexpr bool               is_signed                = true;
    static constexpr float_round_style  round_style              = round_to_nearest;
    static constexpr int                radix                    = 2;

    static constexpr int                digits                   = 53;
    static constexpr int                digits10                 = 15;
    static constexpr int                max_digits10             = 17;
    static constexpr int                max_exponent             = 1024;
    static constexpr int                max_exponent10           = 308;
    static constexpr int                min_exponent             = -1021;
    static constexpr int                min_exponent10           = -307;

    static constexpr ::math::sd::float64_t (min)        () noexcept { return ::math::sd::float64_t::my_value_min(); }
    static constexpr ::math::sd::float64_t (max)        () noexcept { return ::math::sd::float64_t::my_value_max(); }
    static constexpr ::math::sd::float64_t lowest       () noexcept { return ::math::sd::float64_t::my_value_lowest(); }
    static constexpr ::math::sd::float64_t epsilon      () noexcept { return ::math::sd::float64_t::my_value_epsilon(); }
    static constexpr ::math::sd::float64_t round_error  () noexcept { return ::math::sd::float64_t::my_value_round_error(); }
    static constexpr ::math::sd::float64_t denorm_min   () noexcept { return ::math::sd::float64_t::my_value_denorm_min(); }
    static constexpr ::math::sd::float64_t infinity     () noexcept { return ::math::sd::float64_t::my_value_infinity(); }
    static constexpr ::math::sd::float64_t quiet_NaN    () noexcept { return ::math::sd::float64_t::my_value_quiet_NaN(); }
    static constexpr ::math::sd::float64_t signaling_NaN() noexcept { return ::math::sd::float64_t::my_value_signaling_NaN(); }
  };

  }

#endif // SOFT_DOUBLE_2020_10_27_H_
