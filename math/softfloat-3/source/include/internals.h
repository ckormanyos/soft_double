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

#ifndef INTERNALS_2020_11_13_H_
  #define INTERNALS_2020_11_13_H_

  #include <cstdint>

  #ifdef LITTLEENDIAN
  struct uint128      { uint64_t v0, v64; };
  struct uint64_extra { uint64_t extra, v; };
  #else
  struct uint128      { uint64_t v64, v0; };
  struct uint64_extra { uint64_t v, extra; };
  #endif

  uint32_t softfloat_roundToUI32(bool, uint64_t);
  uint64_t softfloat_roundToUI64(bool, uint64_t);
   int32_t softfloat_roundToI32 (bool, uint64_t);
   int64_t softfloat_roundToI64 (bool, uint64_t);

  uint64_t softfloat_roundPackToF64    (bool, int_fast16_t, uint64_t);
  uint64_t softfloat_normRoundPackToF64(bool, int_fast16_t, uint64_t);

  uint64_t softfloat_addMagsF64(uint64_t, uint64_t, bool);
  uint64_t softfloat_subMagsF64(uint64_t, uint64_t, bool);

  constexpr bool         signF32UI(uint32_t a) { return ((bool) ((uint32_t) (a)>>31)); }
  constexpr int_fast16_t expF32UI (uint32_t a) { return ((int_fast16_t) ((a)>>23) & 0xFF); }
  constexpr uint32_t     fracF32UI(uint32_t a) { return ((a) & 0x007FFFFF); }

  constexpr bool         signF64UI(uint64_t a) { return ((bool) ((uint64_t) (a)>>63)); }
  constexpr int_fast16_t expF64UI (uint64_t a) { return ((int_fast16_t) ((a)>>52) & 0x7FF); }
  constexpr uint64_t     fracF64UI(uint64_t a) { return ((a) & UINT64_C( 0x000FFFFFFFFFFFFF )); }

  template<typename IntegralTypeExp,
           typename IntegralTypeSig>
  constexpr uint64_t packToF64UI(bool sign, IntegralTypeExp exp, IntegralTypeSig sig)
  {
    return ((uint64_t) ((((uint64_t) (sign ? 1ULL : 0ULL))<<63) + (((uint64_t) (exp))<<52) + (uint64_t) (sig)));
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
  constexpr uint_least8_t softfloat_countLeadingZeros8(const uint_fast8_t index)
  {
    const uint_least8_t z_table[16U] =
    {
      4, 3, 2, 2,
      1, 1, 1, 1,
      0, 0, 0, 0,
      0, 0, 0, 0
    };

    return (index < 0x10U) ? 4U + z_table[ index & 0xFU]
                           :      z_table[(index >> 4U)];
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
  uint32_t softfloat_approxRecipSqrt32_1(uint32_t oddExpA, uint32_t a);

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
  static inline struct uint64_extra softfloat_shiftRightJam64Extra(uint64_t a, uint64_t extra, uint32_t dist)
  {
    struct uint64_extra z;

    if(dist < 64)
    {
      z.v = a >> dist;
      z.extra = a << (-dist & 63);
    }
    else
    {
      z.v = 0;
      z.extra = (dist == 64) ? a : (a != 0);
    }

    z.extra |= (extra != 0);
    return z;
  }

#endif // INTERNALS_2020_11_13_H_
