///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <array>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <limits>
#include <random>

#include <math/softfloat/soft_double.h>

static_assert(sizeof(double) == 8U, // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
              "Error: These tests require 8 byte built-in double for verification");

// For an implementation of libm for 64-bit double precision
// floating point (IEEE-754 format), see http://www.netlib.org/fdlibm

#if(__cplusplus >= 201703L)
namespace math::constants {
#else
namespace math { namespace constants { // NOLINT(modernize-concat-nested-namespaces)
#endif

template<typename float_type>
auto pi() -> float_type
{
  return static_cast<float_type>(3.1415926535897932384626433832795028841972L); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

template<>
auto pi() -> math::softfloat::float64_t
{
  return math::softfloat::float64_t::my_value_pi();
}

#if(__cplusplus >= 201703L)
} // namespace math::constants
#else
} // namespace constants
} // namespace math
#endif

template<typename real_value_type,
         typename real_function_type>
auto derivative(const real_value_type&   x,
                const real_value_type&   dx,
                      real_function_type real_function) -> real_value_type
{
  // Compute the first derivative of the input function
  // using a three-point central difference rule of O(dx^6).

  const real_value_type dx2(dx  + dx);
  const real_value_type dx3(dx2 + dx);

  const real_value_type m1((  real_function(x + dx)
                            - real_function(x - dx))  / 2);
  const real_value_type m2((  real_function(x + dx2)
                            - real_function(x - dx2)) / 4);
  const real_value_type m3((  real_function(x + dx3)
                            - real_function(x - dx3)) / 6);

  const real_value_type fifteen_m1(m1 * 15);
  const real_value_type six_m2    (m2 *  6);
  const real_value_type ten_dx    (dx * 10);

  return ((fifteen_m1 - six_m2) + m3) / ten_dx;
}

namespace local
{
  using eng32_type = std::default_random_engine;
  using eng64_type = std::mt19937_64;

  eng32_type eng32; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp,cert-msc32-c,cert-msc51-cpp)
  eng64_type eng64; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp,cert-msc32-c,cert-msc51-cpp)

  // Mantissa range from 10^15 ... 2^53-1
  std::uniform_int_distribution<std::uint64_t> dst_mantissa(UINT64_C(1000000000000000), UINT64_C(9007199254740991)); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)
  std::uniform_int_distribution<std::uint32_t> dst_exp     (UINT32_C(0),                UINT32_C(36));               // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)
  std::uniform_int_distribution<std::uint32_t> dst_sign    (UINT32_C(0),                UINT32_C(1));                // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)

  auto get_sf_float64_t_and_double(math::softfloat::float64_t& x1, // NOLINT(google-runtime-references,readability-function-cognitive-complexity)
                                   double&                     d1, // NOLINT(google-runtime-references)
                                   const bool                  is_positive = false) -> void
  {
    for(;;)
    {
      // Select a random mantissa scaled within approximately 1.0 ... 9.007199...
      d1 = static_cast<double>(dst_mantissa(eng64)) * 1.0E-15; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

      const auto r_exp = dst_exp(eng32);

      const auto exp_is_neg = (dst_sign(eng32) != 0U);

      const double d_scale =
        exp_is_neg ? r_exp ==  0 ? 1.0E-00 : r_exp ==  1 ? 1.0E-01 : r_exp ==  2 ? 1.0E-02 : r_exp ==  3 ? 1.0E-03 :
                     r_exp ==  4 ? 1.0E-04 : r_exp ==  5 ? 1.0E-05 : r_exp ==  6 ? 1.0E-06 : r_exp ==  7 ? 1.0E-07 :
                     r_exp ==  8 ? 1.0E-08 : r_exp ==  9 ? 1.0E-09 : r_exp == 10 ? 1.0E-10 : r_exp == 11 ? 1.0E-11 :
                     r_exp == 12 ? 1.0E-12 : r_exp == 13 ? 1.0E-13 : r_exp == 14 ? 1.0E-14 : r_exp == 15 ? 1.0E-15 :
                     r_exp == 16 ? 1.0E-16 : r_exp == 17 ? 1.0E-17 : r_exp == 18 ? 1.0E-18 : r_exp == 19 ? 1.0E-19 :
                     r_exp == 20 ? 1.0E-20 : r_exp == 21 ? 1.0E-21 : r_exp == 22 ? 1.0E-22 : r_exp == 23 ? 1.0E-23 :
                     r_exp == 24 ? 1.0E-24 : r_exp == 25 ? 1.0E-25 : r_exp == 26 ? 1.0E-26 : r_exp == 27 ? 1.0E-27 :
                     r_exp == 28 ? 1.0E-28 : r_exp == 29 ? 1.0E-29 : r_exp == 30 ? 1.0E-30 : r_exp == 31 ? 1.0E-31 :
                     r_exp == 32 ? 1.0E-32 : r_exp == 33 ? 1.0E-33 : r_exp == 34 ? 1.0E-34 : r_exp == 35 ? 1.0E-35 : 1.0E-36
                   : r_exp ==  0 ? 1.0E+00 : r_exp ==  1 ? 1.0E+01 : r_exp ==  2 ? 1.0E+02 : r_exp ==  3 ? 1.0E+03 :
                     r_exp ==  4 ? 1.0E+04 : r_exp ==  5 ? 1.0E+05 : r_exp ==  6 ? 1.0E+06 : r_exp ==  7 ? 1.0E+07 :
                     r_exp ==  8 ? 1.0E+08 : r_exp ==  9 ? 1.0E+09 : r_exp == 10 ? 1.0E+10 : r_exp == 11 ? 1.0E+11 :
                     r_exp == 12 ? 1.0E+12 : r_exp == 13 ? 1.0E+13 : r_exp == 14 ? 1.0E+14 : r_exp == 15 ? 1.0E+15 :
                     r_exp == 16 ? 1.0E+16 : r_exp == 17 ? 1.0E+17 : r_exp == 18 ? 1.0E+18 : r_exp == 19 ? 1.0E+19 :
                     r_exp == 20 ? 1.0E+20 : r_exp == 21 ? 1.0E+21 : r_exp == 22 ? 1.0E+22 : r_exp == 23 ? 1.0E+23 :
                     r_exp == 24 ? 1.0E+24 : r_exp == 25 ? 1.0E+25 : r_exp == 26 ? 1.0E+26 : r_exp == 27 ? 1.0E+27 :
                     r_exp == 28 ? 1.0E+28 : r_exp == 29 ? 1.0E+29 : r_exp == 30 ? 1.0E+30 : r_exp == 31 ? 1.0E+31 :
                     r_exp == 32 ? 1.0E+32 : r_exp == 33 ? 1.0E+33 : r_exp == 34 ? 1.0E+34 : r_exp == 35 ? 1.0E+35 : 1.0E+36;

      d1 *= d_scale;

      if((!is_positive) && (dst_sign(eng32) != 0U))
      {
        d1 = -d1;
      }

      const auto d_is_ok = (    std::isfinite(d1)
                            && (d1 > static_cast<double>( std::numeric_limits<float>::lowest()))
                            && (d1 < static_cast<double>((std::numeric_limits<float>::max)())));

      if(d_is_ok)
      {
        x1 = math::softfloat::soft_double(math::softfloat::detail::uz_type<double>(d1).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                          math::softfloat::detail::nothing());

        break;
      }
    }
  }
} // namespace local

auto add____sf_float64_t_and_double(math::softfloat::float64_t& x_result, const math::softfloat::float64_t& xa, const math::softfloat::float64_t& xb,         // NOLINT(google-runtime-references,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
                                    double&                     d_result, const double&                     da, const double&                     db) -> void // NOLINT(google-runtime-references)
{
  x_result = xa + xb;
  d_result = da + db;
}

auto sub____sf_float64_t_and_double(math::softfloat::float64_t& x_result, const math::softfloat::float64_t& xa, const math::softfloat::float64_t& xb,         // NOLINT(google-runtime-references,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
                                    double&                     d_result, const double&                     da, const double&                     db) -> void // NOLINT(google-runtime-references)
{
  x_result = xa - xb;
  d_result = da - db;
}

auto mul____sf_float64_t_and_double(math::softfloat::float64_t& x_result, const math::softfloat::float64_t& xa, const math::softfloat::float64_t& xb,         // NOLINT(google-runtime-references,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
                                    double&                     d_result, const double&                     da, const double&                     db) -> void // NOLINT(google-runtime-references)
{
  x_result = xa * xb;
  d_result = da * db;
}

auto div____sf_float64_t_and_double(math::softfloat::float64_t& x_result, const math::softfloat::float64_t& xa, const math::softfloat::float64_t& xb,         // NOLINT(google-runtime-references,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
                                    double&                     d_result, const double&                     da, const double&                     db) -> void // NOLINT(google-runtime-references)
{
  x_result = xa / xb;
  d_result = da / db;
}

auto sqrt___sf_float64_t_and_double(math::softfloat::float64_t& x_result, const math::softfloat::float64_t& xa,         // NOLINT(google-runtime-references,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
                                    double&                     d_result, const double&                     da) -> void // NOLINT(google-runtime-references)
{
  using std::sqrt;

  x_result = sqrt(xa);
  d_result = sqrt(da);
}

auto test_to_f32(const std::uint32_t n) -> bool
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    math::softfloat::float64_t x { };
    double                     d { };

    local::get_sf_float64_t_and_double(x, d);

    const auto f_x = static_cast<float>(x);
    const auto f_d = static_cast<float>(d);

    const uint32_t u_x = math::softfloat::detail::uz_type<float>(f_x).my_u; // NOLINT(cppcoreguidelines-pro-type-union-access)
    const uint32_t u_d = math::softfloat::detail::uz_type<float>(f_d).my_u; // NOLINT(cppcoreguidelines-pro-type-union-access)

    result_is_ok &= (u_x == u_d);
  }

  return result_is_ok;
}

auto test_frexp(const std::uint32_t n) -> bool
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    math::softfloat::float64_t x  { };
    int                        nx { };
    double                     d  { };
    int                        nd { };

    local::get_sf_float64_t_and_double(x, d);

    using std::frexp;

    const math::softfloat::float64_t fr_d = frexp(x, &nd);
    const double                     fr_x = frexp(d, &nx);

    result_is_ok &=
      (   (math::softfloat::detail::uz_type<double>(fr_x).my_u == fr_d.crepresentation()) // NOLINT(cppcoreguidelines-pro-type-union-access)
       && (nx == nd));
  }

  return result_is_ok;
}

auto test_ldexp(const std::uint32_t n) -> bool
{
  const auto s = std::random_device().operator()();

  std::mt19937 eng32(s);

  std::uniform_int_distribution<std::uint32_t> dst_exp_unsigned(UINT32_C(0), UINT32_C(2001));

  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    math::softfloat::float64_t x { }; int nx { };
    double                     d { }; int nd { };

    local::get_sf_float64_t_and_double(x, d);

    using std::frexp;

    const math::softfloat::float64_t fr_d = frexp(x, &nd);
    const double                     fr_x = frexp(d, &nx);

    const std::mt19937::result_type r_exp = dst_exp_unsigned(eng32);

    nx = static_cast<int>(r_exp) - 1001; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    nd = nx;

    using std::ldexp;

    const math::softfloat::float64_t ld_d = ldexp(fr_d, nd);
    const double                     ld_x = ldexp(fr_x, nx);

    result_is_ok &=
      (   (math::softfloat::detail::uz_type<double>(ld_x).my_u == ld_d.crepresentation()) // NOLINT(cppcoreguidelines-pro-type-union-access)
       && (nx == nd));
  }

  return result_is_ok;
}

auto test_exp() -> bool
{
  bool result_is_ok = true;

  const auto x201 = math::softfloat::float64_t(201);

  for(int i = 1; i < 400; ++i) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  {
    const math::softfloat::float64_t x = x201 / math::softfloat::float64_t(i);
    const auto                       d = static_cast<double>(x);

    using std::exp;

    const math::softfloat::float64_t e_x = exp(x);
    const double                     e_d = exp(d);

    const double closeness = std::fabs(1.0 - std::fabs(static_cast<double>(e_x) / e_d));

    result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  }

  for(int i = 1; i < 400; ++i) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  {
    const math::softfloat::float64_t x = -x201 / math::softfloat::float64_t(i);
    const auto                       d = static_cast<double>(x);

    using std::exp;

    const math::softfloat::float64_t e_x = exp(x);
    const double                     e_d = exp(d);

    using std::fabs;

    const auto closeness = fabs(1.0 - fabs(static_cast<double>(e_x) / e_d));

    result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  }

  return result_is_ok;
}

auto test_log() -> bool
{
  bool result_is_ok = true;

  for(std::uint32_t i = 1U; i < 100U; ++i) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  {
    const auto d = static_cast<double>(static_cast<double>(i) / 100.0);

    math::softfloat::float64_t x =
      math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                 math::softfloat::detail::nothing());

    using std::log;

    const math::softfloat::float64_t l_d = log(x);
    const double                     l_x = log(d);

    using std::fabs;

    const double closeness = fabs(1.0 - fabs(static_cast<double>(l_d) / l_x));

    result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  }

  for(std::uint32_t i = 1001U; i < 2000U; ++i) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  {
    const auto d = static_cast<double>(static_cast<double>(i) / 1000.0);

    math::softfloat::float64_t x =
      math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                 math::softfloat::detail::nothing());

    using std::log;

    const math::softfloat::float64_t l_d = log(x);
    const double                     l_x = log(d);

    using std::fabs;

    const double closeness = fabs(1.0 - fabs(static_cast<double>(l_d) / l_x));

    result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  }

  using std::fabs;
  using std::log;

  double                     d { };
  math::softfloat::float64_t x;
  math::softfloat::float64_t l_d;
  double                     l_x { };
  double                     closeness { };

  d             = 1.1E1; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  x             = math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                             math::softfloat::detail::nothing());
  l_d           = log(x);
  l_x           = log(d);
  closeness     = fabs(1.0 - fabs(static_cast<double>(l_d) / l_x));
  result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  d             = 1.1E10; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  x             = math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                             math::softfloat::detail::nothing());
  l_d           = log(x);
  l_x           = log(d);
  closeness     = fabs(1.0 - fabs(static_cast<double>(l_d) / l_x));
  result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  d             = 1.1E100; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  x             = math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                             math::softfloat::detail::nothing());
  l_d           = log(x);
  l_x           = log(d);
  closeness     = fabs(1.0 - fabs(static_cast<double>(l_d) / l_x));
  result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 40.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  return result_is_ok;
}

auto test_sin() -> bool
{
  bool result_is_ok = true;

  for(int i = 0; i < 2001; ++i) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  {
    const math::softfloat::float64_t x = math::softfloat::float64_t(i) / 1000;
    const auto                       d = static_cast<double>(x);

    using std::sin;

    const math::softfloat::float64_t s_x = sin(x);
    const double                     s_d = sin(d);

    if(i == 0)
    {
      result_is_ok &= s_x.crepresentation() == 0U;
    }
    else
    {
      const double closeness = std::fabs(1.0 - std::fabs(static_cast<double>(s_x) / s_d));

      result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 50.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    }
  }

  return result_is_ok;
}

auto test_cos() -> bool
{
  bool result_is_ok = true;

  for(int i = 0; i < 1501; ++i) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  {
    const math::softfloat::float64_t x = math::softfloat::float64_t(i) / 1000;
    const auto                       d = static_cast<double>(x);

    using std::cos;

    const math::softfloat::float64_t c_x = cos(x);
    const double                     c_d = cos(d);

    const double closeness = std::fabs(1.0 - std::fabs(static_cast<double>(c_x) / c_d));

    result_is_ok &= (closeness < std::numeric_limits<double>::epsilon() * 60.0); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  }

  return result_is_ok;
}

auto test_floor(const std::uint32_t n) -> bool
{
  bool result_is_ok = true;

  constexpr std::array<double, 8U> powers_of_ten =
  {{
           1.0,
          10.0,
         100.0,
        1000.0,
       10000.0,
      100000.0,
     1000000.0,
    10000000.0
  }};

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    double d { };

    {
      // Select a random mantissa scaled within approximately 0.1 ... 0.9007199...
      const auto dp =
        static_cast<double>
        (
            static_cast<double>
            (
              static_cast<double>(local::dst_mantissa(local::eng64)) * 1.0E-16
            )
          *
            static_cast<double>
            (
              powers_of_ten.at(static_cast<std::size_t>(i % powers_of_ten.size()))
            )
        );

      const bool d_is_neg = (local::dst_sign(local::eng32) != 0U);

      d = d_is_neg ? -dp : dp;
    }

    const math::softfloat::float64_t x =
      math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                 math::softfloat::detail::nothing());

    using std::floor;

    const math::softfloat::float64_t f_x = floor(x);
    const double                     f_d = floor(d);

    result_is_ok &= (static_cast<std::int32_t>(f_x) == static_cast<std::int32_t>(f_d));
  }

  return result_is_ok;
}

auto test_ceil(const std::uint32_t n) -> bool
{
  bool result_is_ok = true;

  constexpr std::array<double, 8U> powers_of_ten =
  {{
           1.0,
          10.0,
         100.0,
        1000.0,
       10000.0,
      100000.0,
     1000000.0,
    10000000.0
  }};

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    double d { };

    {
      // Select a random mantissa scaled within approximately 0.1 ... 0.9007199...
      const auto dp =
        static_cast<double>
        (
            static_cast<double>
            (
              static_cast<double>(local::dst_mantissa(local::eng64)) * 1.0E-16
            )
          * powers_of_ten.at(static_cast<std::size_t>(i % powers_of_ten.size()))
        );

      const auto d_is_neg = (local::dst_sign(local::eng32) != 0U);

      d = d_is_neg ? -dp : dp;
    }

    const math::softfloat::float64_t x =
      math::softfloat::float64_t(math::softfloat::detail::uz_type<double>(d).my_u, // NOLINT(cppcoreguidelines-pro-type-union-access)
                                 math::softfloat::detail::nothing());

    using std::ceil;

    const math::softfloat::float64_t f_x = ceil(x);
    const double                     f_d = ceil(d);

    result_is_ok &= (static_cast<std::int32_t>(f_x) == static_cast<std::int32_t>(f_d));
  }

  return result_is_ok;
}

auto test_ops(const std::uint32_t n, std::uint_fast8_t op_index) -> bool
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    math::softfloat::float64_t xa;
    math::softfloat::float64_t xb;
    math::softfloat::float64_t xr;

    double da { };
    double db { };
    double dr { };

    if     (op_index == 4U) { local::get_sf_float64_t_and_double(xa, da, true);                                       sqrt___sf_float64_t_and_double(xr, xa,     dr, da); }
    else if(op_index == 3U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); div____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }
    else if(op_index == 2U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); mul____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }
    else if(op_index == 1U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); sub____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }
    else                    { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); add____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }

    result_is_ok &= (xr.crepresentation() == math::softfloat::detail::uz_type<double>(dr).my_u); // NOLINT(cppcoreguidelines-pro-type-union-access)

    if(!result_is_ok)
    {
      std::cout << dr << std::endl;

      break;
    }
  }

  return result_is_ok;
}

auto test_neq(const std::uint32_t n, std::uint_fast8_t op_index) -> bool
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    math::softfloat::float64_t xa { };
    math::softfloat::float64_t xb { };
    double                     da { };
    double                     db { };

    bool x_result = false;
    bool d_result = true;

    if     (op_index == 0U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); x_result = (xa <  xb); d_result = (da <  db); }
    else if(op_index == 1U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); x_result = (xa >  xb); d_result = (da >  db); }
    else if(op_index == 2U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); x_result = (xa != xb); d_result = (da != db); }

    result_is_ok &= (x_result == d_result);

    if(!result_is_ok)
    {
      std::cout << da << std::endl;
      std::cout << db << std::endl;

      break;
    }
  }

  return result_is_ok;
}

auto test_eq_(const std::uint32_t n, std::uint_fast8_t op_index) -> bool // NOLINT(readability-identifier-naming)
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    math::softfloat::float64_t x;
    double                     d { };

    bool x_ok { };

    if     (op_index == 0U) { local::get_sf_float64_t_and_double(x, d);                                              x_ok = (math::softfloat::detail::uz_type<double>(x.crepresentation()).my_f == d); } // NOLINT(cppcoreguidelines-pro-type-union-access)
    else if(op_index == 1U) { local::get_sf_float64_t_and_double(x, d); const math::softfloat::float64_t x2 = x / 2; x_ok = (math::softfloat::detail::uz_type<double>(x.crepresentation()).my_f <= d) && ((d > 0) ? (math::softfloat::detail::uz_type<double>(x2.crepresentation()).my_f < d) : (math::softfloat::detail::uz_type<double>(x2.crepresentation()).my_f > d)); } // NOLINT(cppcoreguidelines-pro-type-union-access)
    else if(op_index == 2U) { local::get_sf_float64_t_and_double(x, d); const math::softfloat::float64_t x2 = x * 2; x_ok = (math::softfloat::detail::uz_type<double>(x.crepresentation()).my_f <= d) && ((d > 0) ? (math::softfloat::detail::uz_type<double>(x2.crepresentation()).my_f > d) : (math::softfloat::detail::uz_type<double>(x2.crepresentation()).my_f < d)); } // NOLINT(cppcoreguidelines-pro-type-union-access)
    else                    { x_ok = false; d = 0.0; }

    result_is_ok &= x_ok;

    if(!result_is_ok)
    {
      std::cout << d << std::endl;

      break;
    }
  }

  return result_is_ok;
}

auto test_soft_double() -> bool
{
  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  auto result_is_ok = true;

  std::cout << "Start test_soft_double()" << std::endl;

  const auto start = std::clock();

  {
    std::cout << "testing cast___... ";

    const math::softfloat::float64_t xp  = math::constants::pi<math::softfloat::float64_t>();
    const auto u64 = static_cast<std::uint64_t>(xp);
    const auto result_cast___is_ok = (u64 == 3U); // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
    std::cout << std::boolalpha << result_cast___is_ok << std::endl;
    result_is_ok &= result_cast___is_ok;
  }

  {
    std::cout << "testing f32____... ";

    math::softfloat::float64_t xf = math::softfloat::float64_t(0.125F) + math::softfloat::float64_t(0.5F); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    double df = math::softfloat::detail::uz_type<double>(xf.crepresentation()).my_f; // NOLINT(cppcoreguidelines-pro-type-union-access)
    const auto result_f32____is_ok = (df == 0.625); // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
    std::cout << std::boolalpha << result_f32____is_ok << std::endl;
    result_is_ok &= result_f32____is_ok;
  }

  {
    std::cout << "testing neg____... ";

    math::softfloat::float64_t  xn(1 / math::softfloat::float64_t(3));
    xn = -xn;
    double dn = math::softfloat::detail::uz_type<double>(xn.crepresentation()).my_f; // NOLINT(cppcoreguidelines-pro-type-union-access)
    auto result_neg____is_ok = fabs(1.0 - (dn / -0.33333333333333333)) < std::numeric_limits<double>::epsilon(); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

    xn = -xn;
    dn = math::softfloat::detail::uz_type<double>(xn.crepresentation()).my_f; // NOLINT(cppcoreguidelines-pro-type-union-access)
    result_neg____is_ok &= fabs(1.0 - (dn / 0.33333333333333333)) < std::numeric_limits<double>::epsilon(); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    std::cout << std::boolalpha << result_neg____is_ok << std::endl;
    result_is_ok &= result_neg____is_ok;
  }

  {
    std::cout << "testing ddx____... ";

    math::softfloat::float64_t  x(1 / math::softfloat::float64_t(2));
    math::softfloat::float64_t dx(1 / math::softfloat::float64_t(8388608)); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    math::softfloat::float64_t result = derivative(x,
                                      dx,
                                      [](const math::softfloat::float64_t my_x) -> math::softfloat::float64_t
                                      {
                                        return ((my_x * my_x) / math::softfloat::float64_t(3)) + my_x;
                                      });

    double d = math::softfloat::detail::uz_type<double>(result.crepresentation()).my_f; // NOLINT(cppcoreguidelines-pro-type-union-access)

    using std::fabs;

    const auto result_ddx____is_ok = fabs(1.0 - (d / 1.33333333333333333)) < std::numeric_limits<double>::epsilon() * 1.0E6; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
    std::cout << std::boolalpha << result_ddx____is_ok << std::endl;
    result_is_ok &= result_ddx____is_ok;
  }

  std::cout << "testing to_f32_... "; const bool result_to_f32_is_ok = test_to_f32(100000000U);    std::cout << std::boolalpha << result_to_f32_is_ok << std::endl;

  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  std::cout << "testing frexp__... "; const auto result_frexp__is_ok = test_frexp ( 20000000U);     std::cout << std::boolalpha << result_frexp__is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing ldexp__... "; const auto result_ldexp__is_ok = test_ldexp ( 20000000U);     std::cout << std::boolalpha << result_ldexp__is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing exp____... "; const auto result_exp____is_ok = test_exp   ( );              std::cout << std::boolalpha << result_exp____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing log____... "; const auto result_log____is_ok = test_log   ( );              std::cout << std::boolalpha << result_log____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing sin____... "; const auto result_sin____is_ok = test_sin   ( );              std::cout << std::boolalpha << result_sin____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing cos____... "; const auto result_cos____is_ok = test_cos   ( );              std::cout << std::boolalpha << result_cos____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  std::cout << "testing floor__... "; const auto result_floor__is_ok = test_floor (200000000U);     std::cout << std::boolalpha << result_floor__is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  std::cout << "testing ceil___... "; const auto result_ceil___is_ok = test_ceil  (200000000U);     std::cout << std::boolalpha << result_ceil___is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  std::cout << "testing add____... "; const auto result_add____is_ok = test_ops   ( 20000000U, 0U); std::cout << std::boolalpha << result_add____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing sub____... "; const auto result_sub____is_ok = test_ops   ( 20000000U, 1U); std::cout << std::boolalpha << result_sub____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing mul____... "; const auto result_mul____is_ok = test_ops   ( 20000000U, 2U); std::cout << std::boolalpha << result_mul____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing div____... "; const auto result_div____is_ok = test_ops   ( 20000000U, 3U); std::cout << std::boolalpha << result_div____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing sqrt___... "; const auto result_sqrt___is_ok = test_ops   ( 20000000U, 4U); std::cout << std::boolalpha << result_sqrt___is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  std::cout << "testing lt_____... "; const auto result_lt_____is_ok = test_neq   ( 20000000U, 0U); std::cout << std::boolalpha << result_lt_____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing gt_____... "; const auto result_gt_____is_ok = test_neq   ( 20000000U, 1U); std::cout << std::boolalpha << result_gt_____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing neq____... "; const auto result_neq____is_ok = test_neq   ( 20000000U, 2U); std::cout << std::boolalpha << result_neq____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

  local::eng32.seed(static_cast<typename local::eng32_type::result_type>(std::clock()));
  local::eng64.seed(static_cast<typename local::eng64_type::result_type>(~static_cast<uint64_t>(std::clock())));

  std::cout << "testing eq_____... "; const auto result_eq_____is_ok = test_neq   ( 20000000U, 0U); std::cout << std::boolalpha << result_eq_____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing leq____... "; const auto result_leq____is_ok = test_neq   ( 20000000U, 1U); std::cout << std::boolalpha << result_leq____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
  std::cout << "testing geq____... "; const auto result_geq____is_ok = test_neq   ( 20000000U, 2U); std::cout << std::boolalpha << result_geq____is_ok << std::endl; // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)

  const auto stop = std::clock();

  std::cout << "Time test_soft_double(): "
            << float(stop - start) / float(CLOCKS_PER_SEC)
            << std::endl;

  const auto result_algebra_is_ok = (   result_to_f32_is_ok
                                     && result_frexp__is_ok
                                     && result_ldexp__is_ok
                                     && result_exp____is_ok
                                     && result_log____is_ok
                                     && result_sin____is_ok
                                     && result_cos____is_ok
                                     && result_floor__is_ok
                                     && result_ceil___is_ok
                                     && result_add____is_ok
                                     && result_sub____is_ok
                                     && result_mul____is_ok
                                     && result_div____is_ok
                                     && result_sqrt___is_ok
                                     && result_lt_____is_ok
                                     && result_gt_____is_ok
                                     && result_neq____is_ok
                                     && result_eq_____is_ok
                                     && result_leq____is_ok
                                     && result_geq____is_ok);

  const auto result_double_is_ok = (result_is_ok && result_algebra_is_ok);

  std::cout << "result_double_is_ok: " << std::boolalpha << result_double_is_ok << std::endl;

  return result_double_is_ok;
}
