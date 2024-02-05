///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2024.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef SOFT_DOUBLE_EXAMPLES_2021_04_17_H // NOLINT(llvm-header-guard)
  #define SOFT_DOUBLE_EXAMPLES_2021_04_17_H

  #if(__cplusplus >= 201703L)
  namespace math::softfloat {
  #else
  namespace math { namespace softfloat { // NOLINT(modernize-concat-nested-namespaces)
  #endif

  auto example001_roots_sqrt             () -> bool;
  auto example004_bessel_recur           () -> bool;
  auto example005_polylog_series         () -> bool;
  auto example007_catalan_series         () -> bool;
  auto example010_hypergeometric_2f1     () -> bool;
  auto example011_trig_trapezoid_integral() -> bool;
  auto example012_exercise_constexpr     () -> bool;

  #if(__cplusplus >= 201703L)
  } // namespace math::softfloat
  #else
  } // namespace softfloat
  } // namespace math
  #endif

#endif // SOFT_DOUBLE_EXAMPLES_2021_04_17_H
