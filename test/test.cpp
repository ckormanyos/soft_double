#include <ctime>
#include <iostream>

// Build locally for test on MinGW or Strawberry via Git bash.
// cd C:\Users\User\Documents\Ks\PC_Software\NumericalPrograms\ExtendedNumberTypes\soft_double
// g++ -finline-functions -finline-limit=64 -march=native -mtune=native -O3 -Wall -Wextra -pedantic -Wno-strict-aliasing -std=c++14 -I. test/test.cpp test/test_soft_double.cpp test/test_soft_double_with_decwide_t_100k_digit_pi.cpp -o soft_double.exe

extern bool test_soft_double();
extern bool test_soft_double_with_decwide_t_100k_digit_pi();

int main()
{
  const std::clock_t start = std::clock();

  const bool result_is_ok_double = test_soft_double();
  const bool result_is_ok_pi     = test_soft_double_with_decwide_t_100k_digit_pi();

  const std::clock_t stop = std::clock();

  std::cout << "Time total: "
            << (float) (stop - start) / (float) CLOCKS_PER_SEC
            << std::endl;

  const bool result_is_ok = (result_is_ok_double && result_is_ok_pi);

  return (result_is_ok ? 0 : -1);
}
