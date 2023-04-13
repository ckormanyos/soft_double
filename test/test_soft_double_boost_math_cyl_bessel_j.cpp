///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2022 - 2023.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/soft_double

// g++ -finline-functions -march=native -mtune=native -O3 -Wall -Wextra -Wpedantic -Wshadow -Wunused-parameter -Wuninitialized -Wunreachable-code -Winit-self -Wzero-as-null-pointer-constant -std=c++2a -I. -I/mnt/c/boost/boost_1_81_0 test/test_soft_double_boost_math_cyl_bessel_j.cpp -o test_soft_double_boost_math_cyl_bessel_j.exe
// ./test_soft_double_boost_math_cyl_bessel_j.exe

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

#if !defined(BOOST_MATH_STANDALONE)
#define BOOST_MATH_STANDALONE
#endif

#if !defined(BOOST_NO_RTTI)
#define BOOST_NO_RTTI
#endif

#if !defined(BOOST_DISABLE_THREADS)
#define BOOST_DISABLE_THREADS
#endif

#if !defined(BOOST_NO_EXCEPTIONS)
#define BOOST_NO_EXCEPTIONS
#endif

#if !defined(BOOST_MATH_MAX_SERIES_ITERATION_POLICY)
#define BOOST_MATH_MAX_SERIES_ITERATION_POLICY 1000U
#endif

#define BOOST_MATH_PROMOTE_FLOAT_POLICY false

#include <boost/cstdfloat.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <math/softfloat/soft_double.h>

namespace detail
{
  template<typename NumericType>
  auto is_close_fraction(const NumericType a,
                         const NumericType b,
                         const NumericType tol = NumericType(std::numeric_limits<NumericType>::epsilon() * NumericType(100))) -> bool
  {
    using std::fabs;

    const auto ratio = fabs(NumericType((NumericType(1) * a) / b));

    const auto closeness = fabs(NumericType(1 - ratio));

    return (closeness < tol);
  }

} // namespace detail

namespace
{
  using my_float_type = ::math::softfloat::float64_t;

  using my_float_control_array_type = std::array<my_float_type, static_cast<std::size_t>(UINT8_C(10))>;

  // Table[N[BesselJ[123/100, n/10], 36], {n, 1, 10, 1}]

  const auto boost_math_test_control =
    my_float_control_array_type
    {
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.022384094800424078635130529844793803)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.052329252391407531307435184345226877)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.085683437351809687785091267647371436)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.121101214488587461397609703216026929)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.157737296526501682321434739413437594)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.194945869633552745026958864087027370)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.232184726291181620217067657303383795)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.268975614426188781953361485754411729)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.304885613485023234627936201325996814)),
      static_cast<my_float_type>(BOOST_FLOATMAX_C(0.339517879133361150735582864456735830)),
    };

  constexpr auto my_float_control_array_size = static_cast<unsigned>(std::tuple_size<my_float_control_array_type>::value);
}

static_assert(std::numeric_limits<my_float_type>::digits == static_cast<int>(INT8_C(53)),
              "Error: Incorrect float64_t type definition");

extern my_float_type cyj;
extern unsigned      xn;
extern my_float_type v;

auto test_soft_double_boost_math_cyl_bessel_j() -> bool
{
  static auto boost_math_test_n_index      = static_cast<unsigned>(UINT8_C(0));
  static auto boost_math_test_result_is_ok = true;

  const auto x =
    static_cast<my_float_type>
    (
      static_cast<my_float_type>(xn) / static_cast<my_float_type>(BOOST_FLOATMAX_C(10))
    );

  cyj = boost::math::cyl_bessel_j(v, x);

  const auto& cyj_ctrl = boost_math_test_control.at(boost_math_test_n_index);

  boost_math_test_result_is_ok =
    (
         detail::is_close_fraction(cyj, cyj_ctrl)
      && boost_math_test_result_is_ok
    );

  ++xn;

  ++boost_math_test_n_index;

  if(boost_math_test_n_index == my_float_control_array_size)
  {
    boost_math_test_n_index = static_cast<unsigned>(UINT8_C(0));
    xn                      = static_cast<unsigned>(UINT8_C(1));
  }

  return boost_math_test_result_is_ok;
}

auto main() -> int
{
  constexpr auto number_of_tests =
    static_cast<unsigned>
    (
      my_float_control_array_size * static_cast<unsigned>(UINT8_C(3))
    );

  auto result_is_ok = true;

  auto test_index = static_cast<unsigned>(UINT8_C(0));

  for( ; ((test_index < number_of_tests) && result_is_ok); ++test_index)
  {
    result_is_ok = test_soft_double_boost_math_cyl_bessel_j() && result_is_ok;
  }

  result_is_ok = ((test_index == number_of_tests) && result_is_ok);

  {
    const auto flg = std::cout.flags();

    std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;

    std::cout.flags(flg);
  }

  return (result_is_ok ? 0 : -1);
}

my_float_type cyj { };
unsigned      xn = static_cast<unsigned>(UINT8_C(1));
my_float_type v  = static_cast<my_float_type>(BOOST_FLOATMAX_C(1.23));
