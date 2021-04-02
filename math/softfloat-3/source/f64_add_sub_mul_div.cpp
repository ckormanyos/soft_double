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

Copyright 2011, 2012, 2013, 2014 The Regents of the University of California.
All rights reserved.

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

uint64_t f64_add(const uint64_t a, const uint64_t b)
{
  const bool signA = signF64UI(a);
  const bool signB = signF64UI(b);

  if(signA == signB)
  {
    return softfloat_addMagsF64(a, b, signA);
  }
  else
  {
    return softfloat_subMagsF64(a, b, signA);
  }
}

uint64_t f64_sub(const uint64_t a, const uint64_t b)
{
  const bool signA = signF64UI(a);
  const bool signB = signF64UI(b);

  if(signA == signB)
  {
    return softfloat_subMagsF64(a, b, signA);
  }
  else
  {
    return softfloat_addMagsF64(a, b, signA);
  }
}

uint64_t f64_mul(const uint64_t a, const uint64_t b)
{
  const bool         signA = signF64UI(a);
  const int_fast16_t expA  = expF64UI (a);
        uint64_t     sigA  = fracF64UI(a);

  const bool         signB = signF64UI(b);
  const int_fast16_t expB  = expF64UI (b);
        uint64_t     sigB  = fracF64UI(b);

  const bool signZ = signA ^ signB;

  if(((!expA) && (!sigA)) || ((!expB) && (!sigB)))
  {
    return packToF64UI(signZ, 0, 0);
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

    struct uint128 sig128Z;

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

uint64_t f64_div(const uint64_t a, const uint64_t b)
{
  const bool         signA = signF64UI(a);
        int_fast16_t expA  = expF64UI (a);
        uint64_t     sigA  = fracF64UI(a);

  const bool         signB = signF64UI(b);
        int_fast16_t expB  = expF64UI (b);
        uint64_t     sigB  = fracF64UI(b);

  const bool signZ = signA ^ signB;

  if((!expA) && (!sigA))
  {
    return packToF64UI(signZ, 0, 0);
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

    const uint32_t recip32    = softfloat_approxRecip32_1(sigB >> 32U) - 2U;
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
