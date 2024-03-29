///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2018 - 2023.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <numeric>

#define SOFT_DOUBLE_DISABLE_IOSTREAM

#include <math/softfloat/soft_double.h>
#include <math/softfloat/soft_double_examples.h>

namespace local
{
  template<typename T>
  auto hypergeometric_2f1(const T& AP, const T& BP, const T& CP, const T& ZM) -> T // NOLINT(readability-identifier-naming,bugprone-easily-swappable-parameters)
  {
    // Implement a rational approximation of hypergeometric_2f1.
    // This C++11 code uses a computational scheme similar to
    // one shown in Y.L. Luke, "Algorithms for the Comuptation
    // of Mathematical Functions", Academic Press, New York, 1977.
    // The original Fortran77 source can be found in the subroutine
    // R2F1() on pages 167-168. Slight corrections are made here
    // in order to repair one or two type-setting errors in the
    // original publication.

    // Luke's original Fortran77 and algorithmic work are fascinating
    // since they are generic and scalable --- decades ahead of their time.
    // Here, in fact, the calculation is scaled down to 15 decimal digits.
    // In other works of mine, the samp algorithm has been scaled up
    // to 1,001 decimal digits of precision --- proof of the remarkable
    // generic-ness of Luke's ground-breaking algorithms from a past
    // time epoch.

    // Retain some stylistic elements and comments from Luke's
    // original Fortran77 coding style.

    const T Z(-ZM); // NOLINT(readability-identifier-naming)

    const T my_zero(static_cast<unsigned>(UINT8_C(0)));
    const T my_one (static_cast<unsigned>(UINT8_C(1)));

    // C
    // C INITIALIZATION :
    // C

          T SABZ((AP + BP) * Z);             // NOLINT(readability-identifier-naming)
    const T AB   (AP * BP);                  // NOLINT(readability-identifier-naming)
    const T ABZ  (AB * Z);                   // NOLINT(readability-identifier-naming)
    const T ABZ1((Z + ABZ) + SABZ);          // NOLINT(readability-identifier-naming)
    const T ABZ2((ABZ1 + SABZ) + (3U * Z));  // NOLINT(readability-identifier-naming)

    auto A = std::array<T, static_cast<std::size_t>(UINT8_C(4))> { }; // NOLINT(readability-identifier-naming)
    auto B = std::array<T, static_cast<std::size_t>(UINT8_C(4))> { }; // NOLINT(readability-identifier-naming)

    B[0U] = my_one;
    A[0U] = my_one;

    const T CP1(CP + my_one); // NOLINT(readability-identifier-naming)

    B[1U] = my_one + (ABZ1 / (CP * 2U));
    A[1U] = B[1U]  - (ABZ  / CP);

    const T CT1(2U * CP1); // NOLINT(readability-identifier-naming)

    B[2U] = my_one + ((ABZ2 / CT1) * (my_one + ABZ1 / ((-T(6U)) + (CT1 * 3U)))); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    A[2U] = B[2U]  - ((ABZ  / CP)  * (my_one + ((ABZ2 - ABZ1) / CT1)));          // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

    SABZ /= 4U;

    const T Z2(Z / 2U); // NOLINT(readability-identifier-naming)

    auto D = std::array<T, static_cast<std::size_t>(UINT8_C(9))> { }; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers,readability-identifier-naming)

    D[0U] = (((T(UINT8_C(7)) / 2U) - AB) * Z2) - SABZ;     // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[1U] = ABZ1 / 4U;                                     // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[2U] = D[1U] - (SABZ * 2U);                           // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[3U] = CP1 + my_one;                                  // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[4U] = CP1 * D[3U];                                   // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[5U] = CP  * D[4U];                                   // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[6U] = T(3U) / 2U;                                    // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[7U] = T(3U) / 4U;                                    // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    D[8U] = D[7U] * Z;                                     // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

    auto G = std::array<T, static_cast<std::size_t>(UINT8_C(3))> { }; // NOLINT(readability-identifier-naming)

    // C
    // C FOR I=3,...,N , THE VALUES A(I) AND B(I) ARE CALCULATED
    // C USING THE RECURRENCE RELATIONS BELOW.
    // C

    for(auto   XI = static_cast<std::uint_fast16_t>(UINT16_C(3)); // NOLINT(readability-identifier-naming)
               XI < static_cast<std::uint_fast16_t>(UINT16_C(10000));
             ++XI)
    {
      G[2U]  = (D[2U] * D[1U]) / (D[7U] * D[5U]);             // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      D[1U] += (D[8U] + SABZ);                                // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      D[2U] += (D[8U] - SABZ);                                // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      G[2U] *= (D[1U] / D[6U]);                               // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      G[0U]  =  my_one + ((D[1U] + D[0U]) / (D[6U] * D[3U])); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      G[1U]  =  D[1U] / (D[4U] * D[6U]);                      // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      D[7U] += (D[6U] * 2U);                                  // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      ++D[6U];                                                // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      G[1U] *= ((CP1 - XI) - ((D[2U] + D[0U]) / D[6U]));      // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

      // C -----------------------------------------------------------------
      // C THE RECURRENCE RELATIONS FOR A(I) and B(I) ARE AS FOLLOWS
      // C -----------------------------------------------------------------

      A[3U] = std::inner_product(G.crbegin(), G.crend(), A.cbegin(), my_zero);
      B[3U] = std::inner_product(G.crbegin(), G.crend(), B.cbegin(), my_zero);

      // Check if the iteration difference (delta) is within
      // tolerance and break from the recursion if it is.
      // Here we analyze the difference between this iteration
      // result and the previous iteration result using:
      //
      //         |     |(A2/B2)]| |
      // delta = | 1 - |--------| |
      //         |     |(A3/B3) | |
      //
      //         |     |(A2*B3)| |
      //       = | 1 - |-------| |,
      //         |     |(A3*B2)| |
      //
      // where the absolute value of the ratio in the second term
      // is used and the absolute value of delta is used.

      using std::fabs;

      const T ratio = fabs((A[2U] * B[3U]) / (A[3U] * B[2U]));
      const T delta = fabs(my_one - ratio);

      if((XI > static_cast<std::uint_fast16_t>(UINT8_C(7))) && (delta < std::numeric_limits<T>::epsilon()))
      {
        break;
      }

      // Shift the arrays for the next recursion.
      std::copy(A.cbegin() + 1U, A.cend(), A.begin());
      std::copy(B.cbegin() + 1U, B.cend(), B.begin());

      D[8U] +=  Z2;          // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      D[0U] += (D[8U] * 2U); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      D[5U] += (D[4U] * 3U); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
      D[4U] += (D[3U] * 2U); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-avoid-magic-numbers)

      ++D[3U];
    }

    // Return the rational approximation. This is given
    // by the ratio of the final recursions of A and B.
    return A.back() / B.back();
  }
} // namespace local

auto math::softfloat::example010_hypergeometric_2f1() -> bool
{
  static_assert(std::numeric_limits<float64_t>::digits >= static_cast<int>(INT8_C(53)),
                "Error: Incorrect float64_t type definition");

  const auto a = float64_t( float64_t(2U) / 3U);
  const auto b = float64_t( float64_t(4U) / 3U);
  const auto c = float64_t( float64_t(5U) / 7U);
  const auto z = float64_t(-float64_t(3U) / 4U);

  const auto h2f1 = local::hypergeometric_2f1(a, b, c, z);

  // N[Hypergeometric2F1[2/3, 4/3, 5/7, -3/4], 41]
  // 0.50100473608761064038202987077811306637010

  const auto control = float64_t(UINT64_C(0x3FE0083B15946592), math::softfloat::detail::nothing());

  using std::fabs;

  const auto closeness = fabs(1 - fabs(h2f1 / control));

  const auto result_is_ok = (closeness < (std::numeric_limits<float64_t>::epsilon() * 10));

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if defined(WIDE_INTEGER_STANDALONE_EXAMPLE010_HYPERGEOMETRIC_2F1)

#if !defined(SOFT_DOUBLE_DISABLE_IOSTREAM)
#include <iomanip>
#include <iostream>
#endif

constexpr auto example_standalone_foodcafe = static_cast<std::uint32_t>(UINT32_C(0xF00DCAFE));

extern "C"
{
  extern volatile std::uint32_t example_standalone_result;

  auto example_run_standalone       (void) -> bool;
  auto example_get_standalone_result(void) -> bool;

  auto example_run_standalone(void) -> bool
  {
    bool result_is_ok = true;

    for(unsigned i = 0U; i < 64U; ++i)
    {
      result_is_ok &= ::math::softfloat::example010_hypergeometric_2f1();
    }

    example_standalone_result =
      static_cast<std::uint32_t>
      (
        result_is_ok ? example_standalone_foodcafe : static_cast<std::uint32_t>(UINT32_C(0xFFFFFFFF))
      );

    #if !defined(SOFT_DOUBLE_DISABLE_IOSTREAM)
    std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
    #endif

    return result_is_ok;
  }

  auto example_get_standalone_result(void) -> bool
  {
    volatile auto result_is_ok =
      (example_standalone_result == static_cast<std::uint32_t>(UINT32_C(0xF00DCAFE)));

    return result_is_ok;
  }
}

auto main() -> int
{
  auto result_is_ok = true;

  result_is_ok = (::example_run_standalone       () && result_is_ok);
  result_is_ok = (::example_get_standalone_result() && result_is_ok);

  return (result_is_ok ? 0 : -1);
}

extern "C"
{
  volatile std::uint32_t example_standalone_result;
}

#endif
