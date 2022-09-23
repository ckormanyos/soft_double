///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2022.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <iomanip>
#include <iostream>

auto test_soft_double_spot_values() -> bool
{
  auto result_spot_values_is_ok = true;

  std::cout << "test_soft_double_spot_values         : ";

  {
    const auto flg = std::cout.flags();

    std::cout << std::boolalpha << result_spot_values_is_ok << std::endl;

    std::cout.flags(flg);
  }

  return result_spot_values_is_ok;
}
