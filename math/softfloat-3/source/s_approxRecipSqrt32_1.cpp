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

Copyright 2011, 2012, 2013, 2014, 2015, 2016 The Regents of the University of
California.  All rights reserved.

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

#include <array>

#include <internals.h>

constexpr std::array<uint16_t, 16U> softfloat_approxRecipSqrt_1k0s =
{{
  0xB4C9U, 0xFFABU, 0xAA7DU, 0xF11CU, 0xA1C5U, 0xE4C7U, 0x9A43U, 0xDA29U,
  0x93B5U, 0xD0E5U, 0x8DEDU, 0xC8B7U, 0x88C6U, 0xC16DU, 0x8424U, 0xBAE1U
}};

constexpr std::array<uint16_t, 16U> softfloat_approxRecipSqrt_1k1s =
{{
  0xA5A5U, 0xEA42U, 0x8C21U, 0xC62DU, 0x788FU, 0xAA7FU, 0x6928U, 0x94B6U,
  0x5CC7U, 0x8335U, 0x52A6U, 0x74E2U, 0x4A3EU, 0x68FEU, 0x432BU, 0x5EFDU
}};

uint32_t softfloat_approxRecipSqrt32_1(uint32_t oddExpA, uint32_t a)
{
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

  uint32_t r = ((uint32_t) r0 << 16U) + ((r0 * (uint64_t) sigma0) >> 25U);

  const uint32_t sqrSigma0 = ((uint64_t) sigma0 * sigma0) >> 32U;

  r += ((uint32_t) ((r >> 1U) + (r >> 3U) - ((uint32_t) r0 << 14U)) * (uint64_t) sqrSigma0) >> 48U;

  if(!(r & UINT32_C(0x80000000)))
  {
    r = UINT32_C(0x80000000);
  }

  return r;
}
