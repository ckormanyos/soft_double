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

#include <stdbool.h>
#include <stdint.h>

#include <internals.h>

uint64_t softfloat_addMagsF64(uint64_t uiA, uint64_t uiB, bool signZ)
{
  const int_fast16_t  expA = expF64UI (uiA);
        uint64_t sigA = fracF64UI(uiA);
  const int_fast16_t  expB = expF64UI (uiB);
        uint64_t sigB = fracF64UI(uiB);

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

      sigA = softfloat_shiftRightJam64(sigA, (uint32_t) (-expDiff));
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

      sigB = softfloat_shiftRightJam64(sigB, (uint32_t) expDiff);
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
