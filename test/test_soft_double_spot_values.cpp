///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2022.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <iomanip>
#include <iostream>

#include <cmath>

#include <math/softfloat/soft_double.h>

namespace detail
{
  template<typename NumericType>
  auto is_close_fraction(const NumericType a,
                         const NumericType b, // NOLINT(bugprone-easily-swappable-parameters)
                         const NumericType tol = static_cast<NumericType>(std::numeric_limits<NumericType>::epsilon() * static_cast<NumericType>(100))) -> bool
  {
    using std::fabs;

    const auto ratio = fabs(static_cast<NumericType>((static_cast<NumericType>(1) * a) / b));

    const auto closeness = fabs(static_cast<NumericType>(static_cast<NumericType>(1) - ratio));

    return (closeness < tol);
  }

} // namespace detail

auto test_soft_double_spot_values() -> bool
{
  auto result_spot_values_is_ok = true;

  using ::math::softfloat::float64_t;

  const auto x = float64_t { static_cast<int>(INT8_C(1)) } / static_cast<int>(INT8_C(2));

  const auto sinh_x_val = sinh(x);
  const auto cosh_x_val = cosh(x);
  const auto tanh_x_val = cosh(x);

  // N[Sinh[1/2], 20]
  // 0.52109530549374736162
  const auto sinh_x_ctrl = static_cast<float64_t>(static_cast<double>(0.52109530549374736162L));

  // N[Cosh[1/2], 20]
  // 1.1276259652063807852
  const auto cosh_x_ctrl = static_cast<float64_t>(static_cast<double>(1.1276259652063807852L));

  // N[Tanh[1/2], 20]
  // 0.46211715726000975850
  const auto tanh_x_ctrl = static_cast<float64_t>(static_cast<double>(0.46211715726000975850L));

  const auto result_sinh_is_ok = detail::is_close_fraction(sinh_x_val, sinh_x_ctrl, static_cast<float64_t>(static_cast<double>(80.0L)));
  const auto result_cosh_is_ok = detail::is_close_fraction(cosh_x_val, cosh_x_ctrl, static_cast<float64_t>(static_cast<double>(80.0L)));
  const auto result_tanh_is_ok = detail::is_close_fraction(tanh_x_val, tanh_x_ctrl, static_cast<float64_t>(static_cast<double>(150.0L)));

  const auto result_hyp_trig_is_ok = (result_sinh_is_ok && result_cosh_is_ok && result_tanh_is_ok);

  result_spot_values_is_ok = (result_hyp_trig_is_ok && result_spot_values_is_ok);

  return result_spot_values_is_ok;
}
