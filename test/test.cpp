///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2025.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <ctime>
#include <iomanip>
#include <iostream>

#include <test/test_soft_double_examples.h>

// cd /mnt/c/Users/ckorm/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/soft_double

// Build locally for test on g++ with C++20.
// g++ -finline-functions -march=native -mtune=native -O3 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wundef -Wunused-parameter -Wuninitialized -Wunreachable-code -Winit-self -Wzero-as-null-pointer-constant -std=c++20 -I. examples/example001_roots_sqrt.cpp examples/example004_bessel_recur.cpp examples/example005_polylog_series.cpp examples/example007_catalan_series.cpp examples/example010_hypergeometric_2f1.cpp examples/example011_trig_trapezoid_integral.cpp examples/example012_exercise_constexpr.cpp test/test.cpp test/test_soft_double.cpp test/test_soft_double_edge_cases.cpp test/test_soft_double_examples.cpp test/test_soft_double_spot_values.cpp -o soft_double.exe

// cd .tidy/make
// make prepare -f make_tidy_01_generic.gmk
// make tidy -f make_tidy_01_generic.gmk --jobs=8

// cd .gcov/make
// make prepare -f make_gcov_01_generic.gmk MY_ALL_COV=0 MY_CC=g++
// make gcov -f make_gcov_01_generic.gmk --jobs=8 MY_ALL_COV=0 MY_CC=g++

// cd /mnt/c/Users/ckorm/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/soft_double
// PATH=/home/chris/coverity/cov-analysis-linux64-2023.6.2/bin:$PATH
// cov-build --dir cov-int g++ -finline-functions -march=native -mtune=native -O3 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wundef -Wunused-parameter -Wuninitialized -Wunreachable-code -Winit-self -Wzero-as-null-pointer-constant -std=c++14 -I. examples/example001_roots_sqrt.cpp examples/example004_bessel_recur.cpp examples/example005_polylog_series.cpp examples/example007_catalan_series.cpp examples/example010_hypergeometric_2f1.cpp examples/example011_trig_trapezoid_integral.cpp examples/example012_exercise_constexpr.cpp test/test.cpp test/test_soft_double.cpp test/test_soft_double_edge_cases.cpp test/test_soft_double_examples.cpp test/test_soft_double_spot_values.cpp -o soft_double.exe
// tar caf soft_double.bz2 cov-int

extern auto test_soft_double                        () -> bool;
extern auto test_soft_double_edge_cases             () -> bool;
extern auto test_soft_double_spot_values            () -> bool;

auto main() -> int
{
  const auto flg = std::cout.flags();

  const auto start = std::clock();

  const auto result_test_soft_double_edge_cases_is_ok              = test_soft_double_edge_cases();
  const auto result_test_soft_double_spot_values_is_ok             = test_soft_double_spot_values();

  std::cout << "result_test_soft_double_edge_cases_is_ok             : " << std::boolalpha << result_test_soft_double_edge_cases_is_ok              << std::endl;
  std::cout << "result_test_soft_double_spot_values_is_ok            : " << std::boolalpha << result_test_soft_double_spot_values_is_ok             << std::endl;

  const auto result_test_soft_double_examples_is_ok                = test_soft_double_examples();

  std::cout << "result_test_soft_double_examples_is_ok               : " << std::boolalpha << result_test_soft_double_examples_is_ok                << std::endl;

  const auto result_test_soft_double_is_ok                         = test_soft_double();

  const auto stop = std::clock();

  std::cout << "Time total                                           : "
            << static_cast<float>(stop - start) / static_cast<float>(CLOCKS_PER_SEC)
            << std::endl;

  const auto result_all_is_ok =
  (
       result_test_soft_double_examples_is_ok
    && result_test_soft_double_is_ok
    && result_test_soft_double_edge_cases_is_ok
    && result_test_soft_double_spot_values_is_ok
  );

  std::cout << "result_all_is_ok                                     : " << std::boolalpha << result_all_is_ok << std::endl;

  std::cout.flags(flg);

  const auto result_of_main = (result_all_is_ok ? 0 : -1);

  std::cout << "result_of_main: " << result_of_main << std::endl;

  return (result_all_is_ok ? 0 : -1);
}
