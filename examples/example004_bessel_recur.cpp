///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2020 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

static_assert(sizeof(double) == 8U, // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
              "Error: This example requires 8 byte built-in double for verification");

namespace local { namespace detail {

template<typename FloatingPointType>
auto bisect(const std::function<FloatingPointType(const FloatingPointType)>& pfn,
            const FloatingPointType x_lo,
            const FloatingPointType x_hi) -> FloatingPointType
{
  using floating_point_type = FloatingPointType;

  const auto f_lo = pfn(x_lo);
  const auto f_hi = pfn(x_hi);

  const auto f_lo_is_neg = (f_lo < static_cast<floating_point_type>(0.0L));
  const auto f_hi_is_neg = (f_hi < static_cast<floating_point_type>(0.0L));

  // Make sure that there is at least one root in the interval.
  if(f_lo_is_neg == f_hi_is_neg)
  {
    // Bisection interval has no root or it has multiple roots!

    return static_cast<floating_point_type>(static_cast<int>(INT8_C(0))); // NOLINT(readability-redundant-casting)
  }

  // Orient the search such that f > 0 lies at x + dx.
  auto dx  = floating_point_type { };
  auto rtb = floating_point_type { };

  if(f_lo_is_neg)
  {
    dx  = x_hi - x_lo; // LCOV_EXCL_LINE
    rtb = x_lo;        // LCOV_EXCL_LINE
  }
  else
  {
    dx  = x_lo - x_hi;
    rtb = x_hi;
  }

  // Bisection iteration loop, maximum 64 times.
  for(auto   i = static_cast<std::uint_fast8_t>(UINT8_C(0));
             i < static_cast<std::uint_fast8_t>(UINT8_C(64));
           ++i)
  {
    dx /= 2;

    const auto xmid = rtb + dx;
    const auto fmid = pfn(xmid);

    if(fmid <= static_cast<floating_point_type>(0.0L))
    {
      rtb = xmid;
    }

    // Test for convergence.
    using std::fabs;

    if(   (fabs(dx)   < static_cast<floating_point_type>(0.5L)) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
       || (fabs(fmid) < std::numeric_limits<floating_point_type>::epsilon()))
    {
      // Return root.
      return rtb;
    }
  }

  // Bisection iteration did not converge.
  return static_cast<floating_point_type>(static_cast<int>(INT8_C(0)));
}

struct jn_algo
{
  static constexpr auto e_half() -> float { return static_cast<float>(1.359140914229522617680143735676331248879L); } // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  static constexpr auto two_pi() -> float { return static_cast<float>(6.283185307179586476925286766559005768394L); } // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  static float        m_z; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
  static float        m_n; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
  static std::int32_t m_p; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static auto fn_mstart1(const float m) -> float
  {
    // This equation corresponds to | Jm(z) | = 10^(-p)

    //                          1             e * z
    // Jm asymptotic --> ----------------  * [-----]^m = 10^-p
    //                   sqrt(2 * pi * m)     2 * m
    //
    // The resulting root equation is:
    // p + log10(Jnu asymptotic) = 0

    using std::log10;

    return   static_cast<float>(m_p)
            - (0.5F * log10(two_pi() * m)) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            + (  m  * log10(e_half() * m_z / m));
  }

  static auto fn_mstart2(const float m) -> float
  {
    // This equation corresponds to | Jm(z) | = 10^(-p / 2) * | Jn(z)|

    //                          1             e * z                         1             e * z
    // Jm asymptotic --> ----------------  * [-----]^m = 10^(-p/2) * ----------------  * [-----]^n
    //                   sqrt(2 * pi * m)     2 * m                  sqrt(2 * pi * n)     2 * n
    //
    // The resulting root equation is:
    // p + log10(Jnu asymptotic) = 0

    using std::log10;

    return   (static_cast<float>(m_p) / 2.0F)      // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            - (0.5F * log10(two_pi() * m))          // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            + (  m  * log10(e_half() * m_z / m))    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            + (0.5F * log10(two_pi() * m_n))        // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            - (m_n  * log10(e_half() * m_z / m_n)); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  }

  static auto mstart1(const float x, const std::uint32_t digits) -> std::uint32_t // NOLINT(bugprone-easily-swappable-parameters)
  {
    m_z = (std::max)(x, 0.1F); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    m_p = static_cast<std::int32_t>(digits);

    using function_type = std::function<float(const float)>;

    // Get the starting order for recursion.
    const auto f_order = bisect(function_type(fn_mstart1), 0.1F, x + 100000.0F); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    const auto n_order = static_cast<std::uint32_t>(f_order);     // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    // Make sure the return value is an odd integer.
    return ((((n_order % 2U) == 0U) && (n_order > 0U)) ? n_order - 1U : n_order);
  }

  static auto mstart2(const float x, const float dn, const std::uint32_t digits) -> std::uint32_t // NOLINT(bugprone-easily-swappable-parameters)
  {
    m_z = (std::max)(x, 0.1F); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    m_n = dn;
    m_p = static_cast<std::int32_t>(digits);

    using function_type = std::function<float(const float)>;

    // Get the starting order for recursion.
    const auto f_order = bisect(function_type(fn_mstart2), 0.1F, x + 100000.0F); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    const auto n_order = static_cast<std::uint32_t>(f_order);     // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    // Make sure the return value is an odd integer.
    return ((((n_order % 2U) == 0U) && (n_order > 0U)) ? n_order - 1U : n_order);
  }
};

float         jn_algo::m_z; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float         jn_algo::m_n; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
std::int32_t  jn_algo::m_p; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

} // namespace detail

template<typename FloatingPointType>
auto cyl_bessel_j(const std::int32_t n, const FloatingPointType x) -> FloatingPointType
{
  using floating_point_type = FloatingPointType;

  const floating_point_type one_over_x = floating_point_type(1U) / x;

  floating_point_type my_jn_result(0U);

  // Start recursion using two J's, Jn+2 and Jn+1.
  // Arbitrarily set Jn+2 = 0 and Jn+1 = 1.
  floating_point_type jn_p2    (0U);
  floating_point_type jn_p1    (1U);
  floating_point_type norm_half(1U);

  constexpr auto d10 = static_cast<std::uint32_t>(std::numeric_limits<floating_point_type>::digits10);

  // Get the starting order for recursive calculations.
  const auto n_start1 = static_cast<std::int32_t>(detail::jn_algo::mstart1(static_cast<float>(x), d10));
  const auto n_start2 = static_cast<std::int32_t>(detail::jn_algo::mstart2(static_cast<float>(x), static_cast<float>(n - 1), d10));

  const auto n_start = (std::max)(n_start2, n_start1);

  // Do the recursion. The direction of the recursion is downward.
  for(auto m = n_start; m >= static_cast<std::int32_t>(INT8_C(0)); --m) // NOLINT(altera-id-dependent-backward-branch)
  {
    //                                 Jn+1(x)
    // Downward recursion is:  Jn(x) = ------- * [2 * (m + 1)] - Jn+2(x)
    //                                    x
    const floating_point_type jn = ((jn_p1 * one_over_x) * (floating_point_type(2U) * (m + 1))) - jn_p2;

    jn_p2 = jn_p1;
    jn_p1 = jn;

    // For Normalization use a Neumann expansion of the form
    // 1 = J_0(x) + 2 * J_2(x) + 2 * J_4(x) + 2 * J_6(x) + ...

    if((m % 2) == 0)
    {
      norm_half += ((m == 0) ? jn / 2 : jn);
    }

    // Store the requested value of jn in the result.
    if(m == n)
    {
      my_jn_result = jn;
    }
  }

  // Divide by the normalization in order to get the scaled result.

  return my_jn_result / (norm_half * 2);
}
} // namespace local

auto math::softfloat::example004_bessel_recur() -> bool
{
  // Calculate the double value of a Bessel function.
  // N[BesselJ[9, 123456789/10000000], 40]
  // 0.1811741425937681762466579147691209001166;

  // Downward recursion and simultaneous summation of
  // the normalization factor.
  const float64_t x = float64_t(UINT32_C(123456789)) / float64_t(UINT32_C(10000000));

  const float64_t j9 = local::cyl_bessel_j(9, x);

  const float64_t control(0.1811741425937681762466579147691209001166);

  using std::fabs;

  const float64_t closeness = fabs(1 - (j9 / control));

  const bool result_is_ok = (closeness < (std::numeric_limits<float64_t>::epsilon() * 10));

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0 // NOLINT(readability-avoid-unconditional-preprocessor-if)

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = math::wide_decimal::example004_bessel_recur();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif

