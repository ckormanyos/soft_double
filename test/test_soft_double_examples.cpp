#include <iomanip>
#include <iostream>

#include <math/soft_double/soft_double_examples.h>

bool test_soft_double_examples()
{
  bool result_examples_is_ok = true;

  std::cout << "Start test_soft_double_examples()" << std::endl;

  result_examples_is_ok &= math::sd::example001_roots_sqrt(); std::cout << "example001_roots_sqrt   : " << std::boolalpha << result_examples_is_ok << std::endl;

  std::cout << "result_examples_is_ok: " << std::boolalpha << result_examples_is_ok << std::endl;

  return result_examples_is_ok;
}
