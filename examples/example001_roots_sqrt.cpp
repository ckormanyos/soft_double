///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

bool math::softfloat::example001_roots_sqrt()
{
  using float64_t = ::math::softfloat::float64_t;

  // Use a cached value for pi.
  const float64_t my_pi = float64_t::my_value_pi();

  // Compute soft_double sqrt(pi).
  const float64_t s = sqrt(my_pi);

  using std::sqrt;

  // Compare with native double sqrt(pi).
  const bool result_is_ok =
    (s.crepresentation() == math::softfloat::detail::uz_type<double>(sqrt(3.1415926535897932384626433832795028841972)).my_u);

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = math::softfloat::example001_roots_sqrt();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
