///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <array>
#include <cstdint>
#include <ctime>
#include <limits>
#include <iostream>
#include <random>

#include <math/soft_double/soft_double.h>

using local_softfloat_type = ::math::softfloat::float64_t;

// For an implementation of libm for 64-bit double precision
// floating point (IEEE-754 format), see http://www.netlib.org/fdlibm

namespace math
{
  namespace constants
  {
    template<typename float_type>
    float_type pi()
    {
      return static_cast<float_type>(3.1415926535897932384626433832795028841972L);
    }

    template<>
    ::math::softfloat::float64_t pi()
    {
      return ::math::softfloat::float64_t::my_value_pi();
    }
  }
}

template<typename real_value_type,
         typename real_function_type>
real_value_type derivative(const real_value_type& x,
                           const real_value_type& dx,
                           real_function_type real_function)
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
  const auto s = std::random_device().operator()();

  std::mt19937    eng32(s);
  std::mt19937_64 eng64(s);

  std::uniform_int_distribution<std::uint64_t> dst_mantissa(UINT64_C(10000000000000000), UINT64_C(99999999999999999));
  std::uniform_int_distribution<std::uint32_t> dst_exp     (UINT32_C(0),                 UINT32_C(18));
  std::uniform_int_distribution<std::uint32_t> dst_sign    (UINT32_C(0),                 UINT32_C(1));

  double to_double(::math::softfloat::float64_t x)
  {
    union
    {
      double   d;
      uint64_t u;
    } uZ;

    uZ.u = x.crepresentation();

    return uZ.d;
  }

  void get_sf_float64_t_and_double(::math::softfloat::float64_t& x1, double& d1, const bool is_positive = false)
  {
    d1 = (double) dst_mantissa(eng64);

    const std::mt19937::result_type r_exp = dst_exp(eng32);

    const std::mt19937::result_type sign_of_exp = dst_sign(eng32);

    if(sign_of_exp != 0)
    {
      switch(r_exp)
      {
        default:
        case  0: break;
        case  1: d1 = (1.0 / d1) * 1.0E-01; break;
        case  2: d1 = (1.0 / d1) * 1.0E-02; break;
        case  3: d1 = (1.0 / d1) * 1.0E-03; break;
        case  4: d1 = (1.0 / d1) * 1.0E-04; break;
        case  5: d1 = (1.0 / d1) * 1.0E-05; break;
        case  6: d1 = (1.0 / d1) * 1.0E-06; break;
        case  7: d1 = (1.0 / d1) * 1.0E-07; break;
        case  8: d1 = (1.0 / d1) * 1.0E-08; break;
        case  9: d1 = (1.0 / d1) * 1.0E-09; break;
        case 10: d1 = (1.0 / d1) * 1.0E-10; break;
        case 11: d1 = (1.0 / d1) * 1.0E-11; break;
        case 12: d1 = (1.0 / d1) * 1.0E-12; break;
        case 13: d1 = (1.0 / d1) * 1.0E-13; break;
        case 14: d1 = (1.0 / d1) * 1.0E-14; break;
        case 15: d1 = (1.0 / d1) * 1.0E-15; break;
        case 16: d1 = (1.0 / d1) * 1.0E-16; break;
        case 17: d1 = (1.0 / d1) * 1.0E-17; break;
        case 18: d1 = (1.0 / d1) * 1.0E-18; break;
      }
    }
    else
    {
      switch(r_exp)
      {
        default:
        case  0: break;
        case  1: d1 = d1 * 1.0E+01; break;
        case  2: d1 = d1 * 1.0E+02; break;
        case  3: d1 = d1 * 1.0E+03; break;
        case  4: d1 = d1 * 1.0E+04; break;
        case  5: d1 = d1 * 1.0E+05; break;
        case  6: d1 = d1 * 1.0E+06; break;
        case  7: d1 = d1 * 1.0E+07; break;
        case  8: d1 = d1 * 1.0E+08; break;
        case  9: d1 = d1 * 1.0E+09; break;
        case 10: d1 = d1 * 1.0E+10; break;
        case 11: d1 = d1 * 1.0E+11; break;
        case 12: d1 = d1 * 1.0E+12; break;
        case 13: d1 = d1 * 1.0E+13; break;
        case 14: d1 = d1 * 1.0E+14; break;
        case 15: d1 = d1 * 1.0E+15; break;
        case 16: d1 = d1 * 1.0E+16; break;
        case 17: d1 = d1 * 1.0E+17; break;
        case 18: d1 = d1 * 1.0E+18; break;
      }
    }

    if(is_positive == false)
    {
      const std::mt19937::result_type sign_of_double = dst_sign(eng32);

      if(sign_of_double != 0U)
      {
        d1 = -d1;
      }
    }

    *(double*) &x1 = d1;
  }
}

void add____sf_float64_t_and_double(::math::softfloat::float64_t& x_result, const ::math::softfloat::float64_t& xa, const ::math::softfloat::float64_t& xb,
                                    double&                d_result, const double&                da, const double&                db)
{
  x_result = xa + xb;
  d_result = da + db;
}

void sub____sf_float64_t_and_double(::math::softfloat::float64_t& x_result, const ::math::softfloat::float64_t& xa, const ::math::softfloat::float64_t& xb,
                                    double&                d_result, const double&                da, const double&                db)
{
  x_result = xa - xb;
  d_result = da - db;
}

void mul____sf_float64_t_and_double(::math::softfloat::float64_t& x_result, const ::math::softfloat::float64_t& xa, const ::math::softfloat::float64_t& xb,
                                    double&                d_result, const double&                da, const double&                db)
{
  x_result = xa * xb;
  d_result = da * db;
}

void div____sf_float64_t_and_double(::math::softfloat::float64_t& x_result, const ::math::softfloat::float64_t& xa, const ::math::softfloat::float64_t& xb,
                                    double&                d_result, const double&                da, const double&                db)
{
  x_result = xa / xb;
  d_result = da / db;
}

void sqrt___sf_float64_t_and_double(::math::softfloat::float64_t& x_result, const ::math::softfloat::float64_t& xa,
                                    double&                d_result, const double&                da)
{
  using std::sqrt;

  x_result = sqrt(xa);
  d_result = sqrt(da);
}

bool test_to_f32(const std::uint32_t n)
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    ::math::softfloat::float64_t x;
    double                       d;

    local::get_sf_float64_t_and_double(x, d);

    const float f_x = (float) x;
    const float f_d = (float) d;

    union
    {
      float    f;
      uint32_t u;
    } uZ_x;

    union
    {
      float    f;
      uint32_t u;
    } uZ_d;

    uZ_x.f = f_x;
    uZ_d.f = f_d;

    result_is_ok &= (uZ_x.u == uZ_d.u);
  }

  return result_is_ok;
}

bool test_ops(const std::uint32_t n, std::uint_fast8_t op_index)
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    ::math::softfloat::float64_t xa;
    ::math::softfloat::float64_t xb;
    ::math::softfloat::float64_t xr;

    double        da;
    double        db;
    double        dr;

    if     (op_index == 4U) { local::get_sf_float64_t_and_double(xa, da, true);                                       sqrt___sf_float64_t_and_double(xr, xa,     dr, da); }
    else if(op_index == 3U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); div____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }
    else if(op_index == 2U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); mul____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }
    else if(op_index == 1U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); sub____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }
    else                    { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); add____sf_float64_t_and_double(xr, xa, xb, dr, da, db); }

    union
    {
      double   d;
      uint64_t u;
    } uZ;

    uZ.d = dr;

    result_is_ok &= (xr.crepresentation() == uZ.u);

    if(result_is_ok == false)
    {
      std::cout << dr << std::endl;

      break;
    }
  }

  return result_is_ok;
}

bool test_neq(const std::uint32_t n, std::uint_fast8_t op_index)
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    ::math::softfloat::float64_t xa, xb;
    double        da, db;

    bool x_result = false;
    bool d_result = true;

    if     (op_index == 0U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); x_result = (xa <  xb); d_result = (da <  db); }
    else if(op_index == 1U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); x_result = (xa >  xb); d_result = (da >  db); }
    else if(op_index == 2U) { local::get_sf_float64_t_and_double(xa, da); local::get_sf_float64_t_and_double(xb, db); x_result = (xa != xb); d_result = (da != db); }

    result_is_ok &= (x_result == d_result);

    if(result_is_ok == false)
    {
      std::cout << da << std::endl;
      std::cout << db << std::endl;

      break;
    }
  }

  return result_is_ok;
}

bool test_eq_(const std::uint32_t n, std::uint_fast8_t op_index)
{
  bool result_is_ok = true;

  for(std::uint32_t i = 0U; i < n; ++i)
  {
    ::math::softfloat::float64_t x;
    double                       d;

    bool x_ok;

    if     (op_index == 0U) { local::get_sf_float64_t_and_double(x, d);                                                x_ok = (local::to_double(x) == d); }
    else if(op_index == 1U) { local::get_sf_float64_t_and_double(x, d); const ::math::softfloat::float64_t x2 = x / 2; x_ok = (local::to_double(x) <= d) && ((d > 0) ? (local::to_double(x2) < d) : (local::to_double(x2) > d)); }
    else if(op_index == 2U) { local::get_sf_float64_t_and_double(x, d); const ::math::softfloat::float64_t x2 = x * 2; x_ok = (local::to_double(x) <= d) && ((d > 0) ? (local::to_double(x2) > d) : (local::to_double(x2) < d)); }
    else                    { x_ok = false; d = 0.0; }

    result_is_ok &= x_ok;

    if(result_is_ok == false)
    {
      std::cout << d << std::endl;

      break;
    }
  }

  return result_is_ok;
}

bool test_soft_double()
{
  std::cout << "Start test_soft_double()" << std::endl;

  const std::clock_t start = std::clock();

  bool result_is_ok = true;

  {
    std::cout << "testing cast___... ";

    const ::math::softfloat::float64_t xp  = math::constants::pi<::math::softfloat::float64_t>();
    const std::uint64_t u64 = (std::uint64_t) xp;
    const bool result_cast___is_ok = (u64 == 3U);
    std::cout << std::boolalpha << result_cast___is_ok << std::endl;
    result_is_ok &= result_cast___is_ok;
  }

  {
    std::cout << "testing f32____... ";

    ::math::softfloat::float64_t xf = ::math::softfloat::float64_t(0.125F) + ::math::softfloat::float64_t(0.5F);
    double df = local::to_double(xf);
    const bool result_f32____is_ok = (df == 0.625);
    std::cout << std::boolalpha << result_f32____is_ok << std::endl;
    result_is_ok &= result_f32____is_ok;
  }

  {
    std::cout << "testing neg____... ";

    ::math::softfloat::float64_t  xn(1 / ::math::softfloat::float64_t(3));
    xn = -xn;
    double dn = local::to_double(xn);
    bool result_neg____is_ok = fabs(1.0 - (dn / -0.33333333333333333)) < std::numeric_limits<double>::epsilon();

    xn = -xn;
    dn = local::to_double(xn);
    result_neg____is_ok &= fabs(1.0 - (dn / 0.33333333333333333)) < std::numeric_limits<double>::epsilon();
    std::cout << std::boolalpha << result_neg____is_ok << std::endl;
    result_is_ok &= result_neg____is_ok;
  }

  {
    std::cout << "testing ddx____... ";

    ::math::softfloat::float64_t  x(1 / ::math::softfloat::float64_t(2));
    ::math::softfloat::float64_t dx(1 / ::math::softfloat::float64_t(8388608));

    ::math::softfloat::float64_t result = derivative(x,
                                      dx,
                                      [](const ::math::softfloat::float64_t my_x) -> ::math::softfloat::float64_t
                                      {
                                        return ((my_x * my_x) / ::math::softfloat::float64_t(3)) + my_x;
                                      });

    double d = local::to_double(result);

    using std::fabs;

    const bool result_ddx____is_ok = fabs(1.0 - (d / 1.33333333333333333)) < std::numeric_limits<double>::epsilon() * 1.0E6;
    std::cout << std::boolalpha << result_ddx____is_ok << std::endl;
    result_is_ok &= result_ddx____is_ok;
  }

  std::cout << "testing to_f32_... "; const bool result_to_f32_is_ok = test_to_f32(10000000U);     std::cout << std::boolalpha << result_to_f32_is_ok << std::endl;
  std::cout << "testing add____... "; const bool result_add____is_ok = test_ops   (10000000U, 0U); std::cout << std::boolalpha << result_add____is_ok << std::endl;
  std::cout << "testing sub____... "; const bool result_sub____is_ok = test_ops   (10000000U, 1U); std::cout << std::boolalpha << result_sub____is_ok << std::endl;
  std::cout << "testing mul____... "; const bool result_mul____is_ok = test_ops   (10000000U, 2U); std::cout << std::boolalpha << result_mul____is_ok << std::endl;
  std::cout << "testing div____... "; const bool result_div____is_ok = test_ops   (10000000U, 3U); std::cout << std::boolalpha << result_div____is_ok << std::endl;
  std::cout << "testing sqrt___... "; const bool result_sqrt___is_ok = test_ops   (10000000U, 4U); std::cout << std::boolalpha << result_sqrt___is_ok << std::endl;
  std::cout << "testing lt_____... "; const bool result_lt_____is_ok = test_neq   (10000000U, 0U); std::cout << std::boolalpha << result_lt_____is_ok << std::endl;
  std::cout << "testing gt_____... "; const bool result_gt_____is_ok = test_neq   (10000000U, 1U); std::cout << std::boolalpha << result_gt_____is_ok << std::endl;
  std::cout << "testing neq____... "; const bool result_neq____is_ok = test_neq   (10000000U, 2U); std::cout << std::boolalpha << result_neq____is_ok << std::endl;
  std::cout << "testing eq_____... "; const bool result_eq_____is_ok = test_neq   (10000000U, 0U); std::cout << std::boolalpha << result_eq_____is_ok << std::endl;
  std::cout << "testing leq____... "; const bool result_leq____is_ok = test_neq   (10000000U, 1U); std::cout << std::boolalpha << result_leq____is_ok << std::endl;
  std::cout << "testing geq____... "; const bool result_geq____is_ok = test_neq   (10000000U, 2U); std::cout << std::boolalpha << result_geq____is_ok << std::endl;

  const std::clock_t stop = std::clock();

  std::cout << "Time test_soft_double(): "
            << float(stop - start) / float(CLOCKS_PER_SEC)
            << std::endl;

  const bool result_algebra_is_ok = (   result_add____is_ok
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

  const bool result_double_is_ok = (result_is_ok && result_algebra_is_ok);

  std::cout << "result_double_is_ok: " << std::boolalpha << result_double_is_ok << std::endl;

  return result_is_ok;
}
