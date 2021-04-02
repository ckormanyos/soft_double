///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

// This work uses (significantly) modified parts of the work mentioned below.

/*----------------------------------------------------------------------------

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
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

#include <softfloat.h>

uint32_t f64_to_ui32(const uint64_t a)
{
  bool          sign = signF64UI(a);
  int_fast16_t  exp  = expF64UI (a);
  uint64_t sig  = fracF64UI(a);

  if(exp)
  {
    sig |= UINT64_C(0x0010000000000000);
  }

  int_fast16_t shiftDist = 0x427 - exp;

  if(0 < shiftDist)
  {
    sig = softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
  }

  return softfloat_roundToUI32(sign, sig);
}

int32_t f64_to__i32(const uint64_t a)
{
  bool          sign = signF64UI(a);
  int_fast16_t  exp  = expF64UI (a);
  uint64_t sig  = fracF64UI(a);

  if(exp)
  {
    sig |= UINT64_C(0x0010000000000000);
  }

  int_fast16_t shiftDist = 0x427 - exp;

  if(0 < shiftDist)
  {
    sig = softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
  }

  return softfloat_roundToI32(sign, sig);
}

uint64_t f64_to_ui64(const uint64_t a)
{
  const bool          sign = signF64UI(a);
  const int_fast16_t  exp  = expF64UI (a);
        uint64_t sig  = fracF64UI(a);

  if(exp)
  {
    sig |= UINT64_C(0x0010000000000000);
  }

  int_fast16_t shiftDist = 0x433 - exp;

  struct uint64_extra sigExtra;

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
    sigExtra = softfloat_shiftRightJam64Extra(sig, 0U, (uint32_t) shiftDist);
  }

  return softfloat_roundToUI64(sign, sigExtra.v);
}

int64_t f64_to__i64(const uint64_t a)
{
  const bool          sign = signF64UI(a);
  const int_fast16_t  exp  = expF64UI (a);
        uint64_t sig  = fracF64UI(a);

  if(exp)
  {
    sig |= UINT64_C(0x0010000000000000);
  }

  int_fast16_t shiftDist = 0x433 - exp;

  struct uint64_extra sigExtra;

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
    sigExtra = softfloat_shiftRightJam64Extra(sig, 0U, (uint32_t) shiftDist);
  }

  return softfloat_roundToI64(sign, sigExtra.v);
}
