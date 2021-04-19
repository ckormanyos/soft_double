///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <iomanip>
#include <iostream>

#include <math/soft_double/soft_double_examples.h>

bool test_soft_double_examples()
{
  bool result_examples_is_ok = true;

  std::cout << "Start test_soft_double_examples()" << std::endl;

  result_examples_is_ok &= math::softfloat::example001_roots_sqrt             (); std::cout << "example001_roots_sqrt                : " << std::boolalpha << result_examples_is_ok << std::endl;
  result_examples_is_ok &= math::softfloat::example004_bessel_recur           (); std::cout << "example004_bessel_recur              : " << std::boolalpha << result_examples_is_ok << std::endl;
  result_examples_is_ok &= math::softfloat::example007_catalan_series         (); std::cout << "example007_catalan_series            : " << std::boolalpha << result_examples_is_ok << std::endl;
  result_examples_is_ok &= math::softfloat::example010_hypergeometric_2f1     (); std::cout << "example010_hypergeometric_2f1        : " << std::boolalpha << result_examples_is_ok << std::endl;
  result_examples_is_ok &= math::softfloat::example011_trig_trapezoid_integral(); std::cout << "example011_trig_trapezoid_integral   : " << std::boolalpha << result_examples_is_ok << std::endl;
                                                                                  std::cout << "result_examples_is_ok                : " << std::boolalpha << result_examples_is_ok << std::endl;

  return result_examples_is_ok;
}
