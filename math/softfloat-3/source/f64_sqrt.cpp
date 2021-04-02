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

Copyright 2011, 2012, 2013, 2014, 2017 The Regents of the University of
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

#include <softfloat.h>

uint64_t f64_sqrt(const uint64_t a)
{
  bool         signA = signF64UI(a);
  int_fast16_t expA  = expF64UI (a);
  uint64_t     sigA  = fracF64UI(a);

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
