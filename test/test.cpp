///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <ctime>
#include <iomanip>
#include <iostream>

#include <test/test_soft_double_examples.h>

// Build locally for test on Ubuntu (WSL).
// cd /mnt/c/Users/User/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/soft_double
// g++     -finline-functions -march=native -mtune=native -O3 -Wall -Wextra -pedantic -std=c++11 -I. examples/example001_roots_sqrt.cpp examples/example002b_pi_100k.cpp examples/example004_bessel_recur.cpp examples/example005_polylog_series.cpp examples/example007_catalan_series.cpp examples/example010_hypergeometric_2f1.cpp examples/example011_trig_trapezoid_integral.cpp test/test.cpp test/test_soft_double.cpp test/test_soft_double_examples.cpp -o soft_double.exe
// clang++ -finline-functions -march=native -mtune=native -O3 -Wall -Wextra -pedantic -std=c++17 -I. examples/example001_roots_sqrt.cpp examples/example002b_pi_100k.cpp examples/example004_bessel_recur.cpp examples/example005_polylog_series.cpp examples/example007_catalan_series.cpp examples/example010_hypergeometric_2f1.cpp examples/example011_trig_trapezoid_integral.cpp test/test.cpp test/test_soft_double.cpp test/test_soft_double_examples.cpp -o soft_double.exe

// cd .tidy/make
// make prepare -f make_tidy_01_generic.gmk
// make tidy -f make_tidy_01_generic.gmk --jobs=8

extern auto test_soft_double            () -> bool;
extern auto test_soft_double_spot_values() -> bool;

auto main() -> int
{
  const auto start = std::clock();

  const auto result_test_soft_double_examples_is_ok    = test_soft_double_examples();
  const auto result_test_soft_double_is_ok             = test_soft_double();
  const auto result_test_soft_double_spot_values_is_ok = test_soft_double_spot_values();

  const auto stop = std::clock();

  std::cout << "Time total: "
            << static_cast<float>(stop - start) / static_cast<float>(CLOCKS_PER_SEC)
            << std::endl;

  const auto result_all_is_ok =
  (
       result_test_soft_double_examples_is_ok
    && result_test_soft_double_is_ok
    && result_test_soft_double_spot_values_is_ok
  );

  std::cout << "result_all_is_ok: " << std::boolalpha << result_all_is_ok << std::endl;

  return (result_all_is_ok ? 0 : -1);
}
