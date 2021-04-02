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


/*----------------------------------------------------------------------------
| Note:  If SoftFloat is made available as a general library for programs to
| use, it is strongly recommended that a platform-specific version of this
| header, "softfloat.h", be created that folds in "softfloat_types.h" and that
| eliminates all dependencies on compile-time macros.
-----------------------------------------------------------------------------*/

#ifndef SOFT_FLOAT_2020_11_13_H_
  #define SOFT_FLOAT_2020_11_13_H_

  #include <internals.h>

  uint64_t f64_sqrt(const uint64_t);

  uint64_t f64_add(const uint64_t, const uint64_t);
  uint64_t f64_sub(const uint64_t, const uint64_t);
  uint64_t f64_mul(const uint64_t, const uint64_t);
  uint64_t f64_div(const uint64_t, const uint64_t);

  uint32_t f64_to_ui32(const uint64_t a);
   int32_t f64_to__i32(const uint64_t a);
  uint64_t f64_to_ui64(const uint64_t a);
   int64_t f64_to__i64(const uint64_t a);

  constexpr uint64_t my__i32_to_f64(const int32_t a)
  {
    return
      (!a) ? 0U
           : packToF64UI((a < 0), 0x432 - int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a)) + 21U)), (uint64_t) uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a) << int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a)) + 21U)));
  }

  constexpr uint64_t my__i64_to_f64(const int64_t a)
  {
    return
      (!((uint64_t) a & UINT64_C(0x7FFFFFFFFFFFFFFF)))
        ? ((a < 0) ? packToF64UI(1, 0x43E, 0) : 0U)
        : softfloat_normRoundPackToF64((a < 0), 0x43C, uint64_t((a < 0) ? -(uint64_t) a : (uint64_t) a));
  }

  constexpr uint64_t my_ui32_to_f64(const uint32_t a)
  {
    return
      ((a == 0U)
        ? 0U
        : packToF64UI(0, 0x432 - int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(a) + 21U)), ((uint64_t) a) << int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(a) + 21U))));
  }

  constexpr uint64_t my_ui64_to_f64(const uint64_t a)
  {
    return (!a) ? (uint64_t) 0U
                : (a & UINT64_C(0x8000000000000000)) ? softfloat_roundPackToF64(0, 0x43D, softfloat_shortShiftRightJam64(a, 1))
                                                     : softfloat_normRoundPackToF64(0, 0x43C, a);
  }

#endif // SOFT_FLOAT_2020_11_13_H_
