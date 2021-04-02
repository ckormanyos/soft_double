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

#include <internals.h>

uint64_t softfloat_subMagsF64(uint64_t uiA, uint64_t uiB, bool signZ)
{
  uint64_t     uiZ;
  int_fast16_t expZ;

  int_fast16_t  expA = expF64UI(uiA);
  uint64_t      sigA = fracF64UI(uiA);
  int_fast16_t  expB = expF64UI(uiB);
  uint64_t      sigB = fracF64UI(uiB);

  const int_fast16_t expDiff = expA - expB;

  if(!expDiff)
  {
    int64_t sigDiff = (int64_t) (sigA - sigB);

    if(!sigDiff)
    {
      uiZ = packToF64UI(false, 0, 0);
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

      int_fast8_t shiftDist = (int_fast8_t) (softfloat_countLeadingZeros64((uint64_t) sigDiff) - 11U);

      expZ = expA - shiftDist;

      if(expZ < 0)
      {
        shiftDist = (int_fast8_t) expA;

        expZ = 0;
      }

      uiZ = packToF64UI(signZ, expZ, sigDiff << shiftDist);
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
      sigA  = softfloat_shiftRightJam64(sigA, (uint32_t) -expDiff);

      sigB |= UINT64_C(0x4000000000000000);

      expZ = expB;
      sigZ = sigB - sigA;
    }
    else
    {
      sigB += ((expB != 0) ? UINT64_C(0x4000000000000000) : sigB);
      sigB  = softfloat_shiftRightJam64(sigB, (uint32_t) expDiff);

      sigA |= UINT64_C(0x4000000000000000);

      expZ = expA;
      sigZ = sigA - sigB;
    }

    return softfloat_normRoundPackToF64(signZ, (int_fast16_t) (expZ - 1), sigZ);
  }

  return uiZ;
}
