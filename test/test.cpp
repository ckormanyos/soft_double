#include <ctime>
#include <iomanip>
#include <iostream>

#include <test/test_soft_double_examples.h>

// Build locally for test on MinGW or Strawberry via Git bash.
// cd C:\Users\User\Documents\Ks\PC_Software\NumericalPrograms\ExtendedNumberTypes\soft_double
// g++ -finline-functions -finline-limit=32 -m64 -O3 -Wall -Wextra -pedantic -std=c++11 -I. examples/example001_roots_sqrt.cpp examples/example002b_pi_100k.cpp examples/example004_bessel_recur.cpp examples/example005_polylog_series.cpp examples/example007_catalan_series.cpp examples/example010_hypergeometric_2f1.cpp examples/example011_trig_trapezoid_integral.cpp test/test.cpp test/test_soft_double.cpp test/test_soft_double_examples.cpp -o soft_double.exe

extern bool test_soft_double();

int main()
{
  const std::clock_t start = std::clock();

  const bool result_is_ok_examples = test_soft_double_examples();
  const bool result_is_ok_double   = test_soft_double();

  const std::clock_t stop = std::clock();

  std::cout << "Time total: "
            << (float) (stop - start) / (float) CLOCKS_PER_SEC
            << std::endl;

  const bool result_all_is_ok = (result_is_ok_examples && result_is_ok_double);

  std::cout << "result_all_is_ok: " << std::boolalpha << result_all_is_ok << std::endl;

  return (result_all_is_ok ? 0 : -1);
}
