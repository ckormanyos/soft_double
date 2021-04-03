///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2021.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef SOFT_DOUBLE_2020_10_27_H_
  #define SOFT_DOUBLE_2020_10_27_H_

  #include <cstddef>
  #include <cstdint>
  #include <type_traits>

  #include <internals.h>

  namespace sf {

  namespace detail { struct nothing { }; }

  template<const std::size_t BitCount>
  class soft_double final { };

  template<const std::size_t BitCount> soft_double<64U> operator+(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> soft_double<64U> operator-(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> soft_double<64U> operator*(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> soft_double<64U> operator/(const soft_double<64U>& a, const soft_double<64U>& b);

  template<const std::size_t BitCount, typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double<64U>>::type
  operator+(const soft_double<64U>& u, BuiltinIntegralType n);

  template<const std::size_t BitCount, typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double<64U>>::type
  operator-(const soft_double<64U>& u, BuiltinIntegralType n);

  template<const std::size_t BitCount, typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double<64U>>::type
  operator*(const soft_double<64U>& u, BuiltinIntegralType n);

  template<const std::size_t BitCount, typename BuiltinIntegralType>
  typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                          && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double<64U>>::type
  operator/(const soft_double<64U>& u, BuiltinIntegralType n);

  template<const std::size_t BitCount> bool operator< (const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> bool operator<=(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> bool operator==(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> bool operator!=(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> bool operator>=(const soft_double<64U>& a, const soft_double<64U>& b);
  template<const std::size_t BitCount> bool operator> (const soft_double<64U>& a, const soft_double<64U>& b);

  template<const std::size_t BitCount> bool                   isnan   (const soft_double<64U> x);
  template<const std::size_t BitCount> bool                   isinf   (const soft_double<64U> x);
  template<const std::size_t BitCount> bool                   isfinite(const soft_double<64U> x);
  template<const std::size_t BitCount> soft_double<64U> fabs    (const soft_double<64U> x);
  template<const std::size_t BitCount> soft_double<64U> sqrt    (const soft_double<64U> x);

  template<>
  class soft_double<64U>
  {
  public:
    using representation_type = uint64_t;

    soft_double() { }

    // TBD: Can we use bit twiddling here to attain constexpr construction?
    constexpr soft_double(const std::int8_t  n) : my_value(my__i32_to_f64((std::int32_t) n)) { }
    constexpr soft_double(const std::int16_t n) : my_value(my__i32_to_f64((std::int32_t) n)) { }
    constexpr soft_double(const std::int32_t n) : my_value(my__i32_to_f64(               n)) { }
    constexpr soft_double(const std::int64_t n) : my_value(my__i64_to_f64(               n)) { }

    constexpr soft_double(const std::uint8_t  u) : my_value(my_ui32_to_f64((std::uint32_t) u)) { }
    constexpr soft_double(const std::uint16_t u) : my_value(my_ui32_to_f64((std::uint32_t) u)) { }
    constexpr soft_double(const std::uint32_t u) : my_value(my_ui32_to_f64(                u)) { }
    constexpr soft_double(const std::uint64_t u) : my_value(my_ui64_to_f64(                u)) { }

    static_assert(sizeof(float) == 4U,
                  "Error: This template is designed for 4 byte built-in float");

    soft_double(const float f) : my_value()
    {
      const uint32_t uiA = *((volatile uint32_t*) &f);

      const bool         sign = signF32UI(uiA);
      const int_fast16_t exp  = expF32UI(uiA);
      const uint32_t     frac = fracF32UI(uiA);

      if((!exp) && (!frac))
      {
        my_value = packToF64UI(sign, 0, 0);
      }
      else
      {
        my_value = packToF64UI(sign, exp + 0x380, (uint64_t) frac << 29);
      }
    }

    constexpr soft_double(const soft_double& other) noexcept
      : my_value(other.my_value) { }

    constexpr soft_double(const soft_double&& other) noexcept
      : my_value(other.my_value) { }

    constexpr soft_double(std::uint64_t n, const detail::nothing&) noexcept
      : my_value((std::uint64_t) n) { }

    ~soft_double() = default;

    soft_double& operator=(const soft_double& other)
    {
      if(this != &other)
      {
        my_value = other.my_value;
      }

      return *this;
    }

    soft_double& operator=(soft_double&& other)
    {
      my_value = other.my_value;

      return *this;
    }

          representation_type&  representation()       { return my_value; }
    const representation_type&  representation() const { return my_value; }
    const representation_type& crepresentation() const { return my_value; }

    explicit operator std::int64_t () const { return (std::int64_t)  f64_to__i64(my_value); }
    explicit operator std::uint64_t() const { return (std::uint64_t) f64_to_ui64(my_value); }

    explicit operator std::int32_t () const { return (std::int32_t)  f64_to__i32(my_value); }
    explicit operator std::uint32_t() const { return (std::uint32_t) f64_to_ui32(my_value); }

    soft_double& operator+=(const soft_double& other) { my_value = f64_add(my_value, other.my_value); return *this; }
    soft_double& operator-=(const soft_double& other) { my_value = f64_sub(my_value, other.my_value); return *this; }
    soft_double& operator*=(const soft_double& other) { my_value = f64_mul(my_value, other.my_value); return *this; }
    soft_double& operator/=(const soft_double& other) { my_value = f64_div(my_value, other.my_value); return *this; }

    const soft_double& operator+() const { return *this; }
          soft_double  operator-() const { return soft_double(my_value ^ (uint64_t) (1ULL << 63U), detail::nothing()); }

    static constexpr soft_double my_value_zero() { return soft_double(UINT64_C(0),                   detail::nothing()); }
    static constexpr soft_double my_value_one () { return soft_double(UINT64_C(1),                   detail::nothing()); }
    static constexpr soft_double my_value_pi  () { return soft_double(UINT64_C(4614256656552045848), detail::nothing()); }

    static constexpr soft_double my_value_min()           { return soft_double(UINT64_C(4503599627370496),    detail::nothing()); }
    static constexpr soft_double my_value_max()           { return soft_double(UINT64_C(9218868437227405311), detail::nothing()); }
    static constexpr soft_double my_value_lowest()        { return soft_double(UINT64_C(18442240474082181119),detail::nothing()); }
    static constexpr soft_double my_value_epsilon()       { return soft_double(UINT64_C(4372995238176751616), detail::nothing()); }
    static constexpr soft_double my_value_round_error()   { return soft_double(UINT64_C(4602678819172646912), detail::nothing()); }
    static constexpr soft_double my_value_denorm_min()    { return soft_double(UINT64_C(1),                   detail::nothing()); }
    static constexpr soft_double my_value_infinity()      { return soft_double(UINT64_C(0x7FF0000000000000),  detail::nothing()); }
    static constexpr soft_double my_value_quiet_NaN()     { return soft_double(UINT64_C(0xFFF8000000000000),  detail::nothing()); }
    static constexpr soft_double my_value_signaling_NaN() { return soft_double(UINT64_C(0x7FF8000000000000),  detail::nothing()); }

  private:
    representation_type my_value;

    bool my_le(const soft_double& b) const
    {
      const bool signA = signF64UI(  my_value);
      const bool signB = signF64UI(b.my_value);

      return (signA != signB) ? (signA || !((my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF)))
                              : (my_value == b.my_value) || (signA ^ (my_value < b.my_value));
    }

    bool my_lt(const soft_double& b) const
    {
      const bool signA = signF64UI(  my_value);
      const bool signB = signF64UI(b.my_value);

      return (signA != signB) ? signA && ((my_value | b.my_value) & UINT64_C(0x7FFFFFFFFFFFFFFF))
                              : (my_value != b.my_value) && (signA ^ (my_value < b.my_value));
    }

    static uint64_t f64_add(const uint64_t a, const uint64_t b)
    {
      const bool signA = signF64UI(a);
      const bool signB = signF64UI(b);

      if(signA == signB)
      {
        return softfloat_addMagsF64(a, b, signA);
      }
      else
      {
        return softfloat_subMagsF64(a, b, signA);
      }
    }

    static uint64_t f64_sub(const uint64_t a, const uint64_t b)
    {
      const bool signA = signF64UI(a);
      const bool signB = signF64UI(b);

      if(signA == signB)
      {
        return softfloat_subMagsF64(a, b, signA);
      }
      else
      {
        return softfloat_addMagsF64(a, b, signA);
      }
    }

    static uint64_t f64_mul(const uint64_t a, const uint64_t b)
    {
      const bool         signA = signF64UI(a);
      const int_fast16_t expA  = expF64UI (a);
            uint64_t     sigA  = fracF64UI(a);

      const bool         signB = signF64UI(b);
      const int_fast16_t expB  = expF64UI (b);
            uint64_t     sigB  = fracF64UI(b);

      const bool signZ = signA ^ signB;

      if(((!expA) && (!sigA)) || ((!expB) && (!sigB)))
      {
        return packToF64UI(signZ, 0, 0);
      }
      else
      {
        int_fast16_t expZ = (int_fast16_t) (expA + expB) - 0x3FF;

        sigA = (sigA | UINT64_C(0x0010000000000000)) << 10U;
        sigB = (sigB | UINT64_C(0x0010000000000000)) << 11U;


        // Compute the 128-bit product of sigA and sigB.

        const uint32_t a32 = (uint32_t) (sigA >> 32U);
        const uint32_t a0  = (uint32_t)  sigA;
        const uint32_t b32 = (uint32_t) (sigB >> 32U);
        const uint32_t b0  = (uint32_t)  sigB;

        struct uint128 sig128Z;

        sig128Z.v0 = ((uint64_t) a0) * b0;

        uint64_t mid1 =                    ((uint64_t) a32) * b0;
        uint64_t mid  = mid1 + (uint64_t) (((uint64_t) b32) * a0);

        sig128Z.v64  = ((uint64_t) a32) * b32;
        sig128Z.v64 += ((uint64_t) (mid < mid1) << 32U) | (uint32_t) (mid >> 32U);

        mid <<= 32U;

        sig128Z.v0  += mid;
        sig128Z.v64 += (sig128Z.v0 < mid);

        if(sig128Z.v0 != 0U)
        {
          sig128Z.v64 |= 1U;
        }

        if(sig128Z.v64 < UINT64_C(0x4000000000000000))
        {
          --expZ;

          sig128Z.v64 <<= 1U;
        }

        return softfloat_roundPackToF64(signZ, expZ, sig128Z.v64);
      }
    }

    static uint64_t f64_div(const uint64_t a, const uint64_t b)
    {
      const bool         signA = signF64UI(a);
            int_fast16_t expA  = expF64UI (a);
            uint64_t     sigA  = fracF64UI(a);

      const bool         signB = signF64UI(b);
            int_fast16_t expB  = expF64UI (b);
            uint64_t     sigB  = fracF64UI(b);

      const bool signZ = signA ^ signB;

      if((!expA) && (!sigA))
      {
        return packToF64UI(signZ, 0, 0);
      }
      else
      {
        int_fast16_t expZ = (expA - expB) + 0x3FE;

        sigA |= UINT64_C(0x0010000000000000);
        sigB |= UINT64_C(0x0010000000000000);

        if(sigA < sigB)
        {
          --expZ;

          sigA <<= 11U;
        }
        else
        {
          sigA <<= 10U;
        }

        sigB <<= 11U;

        const uint32_t recip32    = softfloat_approxRecip32_1(sigB >> 32U) - 2U;
        const uint32_t sig32Z     = ((uint32_t) (sigA >> 32U) * (uint64_t) recip32) >> 32U;

        uint32_t doubleTerm = sig32Z << 1U;

        uint64_t rem =   ((sigA - (uint64_t) doubleTerm *  (uint32_t) (sigB >> 32U)) << 28U)
                                - (uint64_t) doubleTerm * ((uint32_t)  sigB >> 4U);

        uint32_t q = (((uint32_t) (rem >> 32U) * (uint64_t) recip32) >> 32U) + 4U;

        uint64_t sigZ = ((uint64_t) sig32Z << 32U) + ((uint64_t) q << 4U);

        if((sigZ & 0x1FF) < (4UL << 4U))
        {
          q    &=  (uint32_t) ~7U;
          sigZ &= ~(uint64_t) 0x7F;

          doubleTerm = q << 1U;

          rem =   ((rem - (uint64_t) doubleTerm *  (uint32_t) (sigB >> 32U)) << 28U)
                -         (uint64_t) doubleTerm * ((uint32_t)  sigB >> 4U);

          if(rem & UINT64_C(0x8000000000000000))
          {
            sigZ -= (1UL << 7U);
          }
          else
          {
            if(rem)
            {
              sigZ |= 1U;
            }
          }
        }

        return softfloat_roundPackToF64(signZ, expZ, sigZ);
      }
    }

    static uint64_t f64_sqrt(const uint64_t a)
    {
      bool         signA = signF64UI(a);
      int_fast16_t expA  = expF64UI (a);
      uint64_t     sigA  = fracF64UI(a);

      if(((!expA) && (!sigA)) || signA)
      {
        return a;
      }
      else
      {
        /*------------------------------------------------------------------------
        | (`sig32Z' is guaranteed to be a lower bound on the square root of
        | `sig32A', which makes `sig32Z' also a lower bound on the square root of
        | `sigA'.)
        *------------------------------------------------------------------------*/
        int_fast16_t expZ = ((expA - 0x3FF) >> 1) + 0x3FE;
        expA &= 1;
        sigA |= UINT64_C(0x0010000000000000);

        uint32_t sig32A      = (uint32_t) (sigA >> 21U);
        uint32_t recipSqrt32 = softfloat_approxRecipSqrt32_1((uint32_t) expA, sig32A);
        uint32_t sig32Z      = ((uint64_t) sig32A * recipSqrt32) >> 32U;

        if(expA)
        {
          sigA   <<= 8U;
          sig32Z >>= 1U;
        }
        else
        {
          sigA <<= 9U;
        }

              uint64_t rem = sigA - (uint64_t) sig32Z * sig32Z;
        const uint32_t q   = ((uint32_t) (rem >> 2U) * (uint64_t) recipSqrt32) >> 32U;

        uint64_t sigZ = ((uint64_t) sig32Z << 32U | (1UL << 5U)) + ((uint64_t) q << 3U);

        if((sigZ & 0x1FFU) < 0x22U)
        {
          sigZ &= ~(uint64_t) 0x3FU;

          const uint64_t shiftedSigZ = sigZ >> 6U;

          rem = (sigA << 52U) - shiftedSigZ * shiftedSigZ;

          if(rem & UINT64_C(0x8000000000000000))
          {
            --sigZ;
          }
          else
          {
            if(rem)
            {
              sigZ |= 1U;
            }
          }
        }

        return softfloat_roundPackToF64(0, expZ, sigZ);
      }
    }

    static uint32_t f64_to_ui32(const uint64_t a)
    {
      bool          sign = signF64UI(a);
      int_fast16_t  exp  = expF64UI (a);
      uint64_t sig  = fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x427 - exp;

      if(0 < shiftDist)
      {
        sig = softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
      }

      return softfloat_roundToUI32(sign, sig);
    }

    static int32_t f64_to__i32(const uint64_t a)
    {
      bool          sign = signF64UI(a);
      int_fast16_t  exp  = expF64UI (a);
      uint64_t sig  = fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x427 - exp;

      if(0 < shiftDist)
      {
        sig = softfloat_shiftRightJam64(sig, (uint32_t) shiftDist);
      }

      return softfloat_roundToI32(sign, sig);
    }

    static uint64_t f64_to_ui64(const uint64_t a)
    {
      const bool          sign = signF64UI(a);
      const int_fast16_t  exp  = expF64UI (a);
            uint64_t sig  = fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x433 - exp;

      struct uint64_extra sigExtra;

      if(shiftDist <= 0)
      {
        if(shiftDist < -11)
        {
          shiftDist = -11;
        }

        sigExtra.v = sig << -shiftDist;
        sigExtra.extra = 0;
      }
      else
      {
        sigExtra = softfloat_shiftRightJam64Extra(sig, 0U, (uint32_t) shiftDist);
      }

      return softfloat_roundToUI64(sign, sigExtra.v);
    }

    static int64_t f64_to__i64(const uint64_t a)
    {
      const bool          sign = signF64UI(a);
      const int_fast16_t  exp  = expF64UI (a);
            uint64_t sig  = fracF64UI(a);

      if(exp)
      {
        sig |= UINT64_C(0x0010000000000000);
      }

      int_fast16_t shiftDist = 0x433 - exp;

      struct uint64_extra sigExtra;

      if(shiftDist <= 0)
      {
        if(shiftDist < -11)
        {
          shiftDist = -11;
        }

        sigExtra.v = sig << -shiftDist;
        sigExtra.extra = 0;
      }
      else
      {
        sigExtra = softfloat_shiftRightJam64Extra(sig, 0U, (uint32_t) shiftDist);
      }

      return softfloat_roundToI64(sign, sigExtra.v);
    }

    static constexpr uint64_t my__i32_to_f64(const int32_t a)
    {
      return
        (!a) ? 0U
             : packToF64UI((a < 0), 0x432 - int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a)) + 21U)), (uint64_t) uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a) << int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(uint32_t((a < 0) ? -(uint32_t) a : (uint32_t) a)) + 21U)));
    }

    static constexpr uint64_t my__i64_to_f64(const int64_t a)
    {
      return
        (!((uint64_t) a & UINT64_C(0x7FFFFFFFFFFFFFFF)))
          ? ((a < 0) ? packToF64UI(1, 0x43E, 0) : 0U)
          : softfloat_normRoundPackToF64((a < 0), 0x43C, uint64_t((a < 0) ? -(uint64_t) a : (uint64_t) a));
    }

    static constexpr uint64_t my_ui32_to_f64(const uint32_t a)
    {
      return
        ((a == 0U)
          ? 0U
          : packToF64UI(0, 0x432 - int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(a) + 21U)), ((uint64_t) a) << int_fast8_t((int_fast8_t) (softfloat_countLeadingZeros32(a) + 21U))));
    }

    static constexpr uint64_t my_ui64_to_f64(const uint64_t a)
    {
      return (!a) ? (uint64_t) 0U
                  : (a & UINT64_C(0x8000000000000000)) ? softfloat_roundPackToF64(0, 0x43D, softfloat_shortShiftRightJam64(a, 1))
                                                       : softfloat_normRoundPackToF64(0, 0x43C, a);
    }

    friend bool isfinite(const soft_double x) { return ((isnan(x) == false) && (isinf(x) == false)); }
    friend bool isnan   (const soft_double x) { return  (x.my_value == my_value_quiet_NaN().my_value); }
    friend bool isinf   (const soft_double x) { return ((x.my_value & my_value_infinity().my_value) == my_value_infinity().my_value); }

    friend inline soft_double fabs(const soft_double x) { return soft_double((((int64_t) x.my_value < 0) ? (uint64_t) (-((int64_t) x.my_value)) : x.my_value), detail::nothing()); }
    friend inline soft_double sqrt(const soft_double x) { return soft_double(f64_sqrt(x.my_value), detail::nothing()); }

    friend inline soft_double operator+(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_add(a.my_value, b.my_value); return result; }
    friend inline soft_double operator-(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_sub(a.my_value, b.my_value); return result; }
    friend inline soft_double operator*(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_mul(a.my_value, b.my_value); return result; }
    friend inline soft_double operator/(const soft_double& a, const soft_double& b) { soft_double result; result.my_value = f64_div(a.my_value, b.my_value); return result; }

    // Global add/sub/mul/div of const decwide_t<MyDigits10, LimbType, AllocatorType, InternalFloatType>& with all built-in types.
    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator+(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) += soft_double(n);
    }

    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator-(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) -= soft_double(n);
    }

    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator*(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) *= soft_double(n);
    }

    template<typename BuiltinIntegralType>
    friend inline typename std::enable_if<   (std::is_integral   <BuiltinIntegralType>::value == true)
                            && (std::is_fundamental<BuiltinIntegralType>::value == true), soft_double>::type
    operator/(const soft_double& u, BuiltinIntegralType n)
    {
      return soft_double(u) /= soft_double(n);
    }

    friend inline bool operator< (const soft_double& a, const soft_double& b) { return a.my_lt(b); }
    friend inline bool operator<=(const soft_double& a, const soft_double& b) { return a.my_le(b); }
    friend inline bool operator==(const soft_double& a, const soft_double& b) { return  (a.my_value == b.my_value); }
    friend inline bool operator!=(const soft_double& a, const soft_double& b) { return ((a == b) == false); }
    friend inline bool operator>=(const soft_double& a, const soft_double& b) { return ((a <  b) == false); }
    friend inline bool operator> (const soft_double& a, const soft_double& b) { return ((a <= b) == false); }
  };

  using float64_t = soft_double<64U>;
  }

  namespace std {

  // TBD: Do not derive directly from numeric_limits,
  // as some members such as is_iec559, etc. should be false.
  template<> class numeric_limits<sf::float64_t> : public std::numeric_limits<double>
  {
  public:
    static constexpr sf::float64_t (min)        () noexcept { return sf::float64_t::my_value_min(); }
    static constexpr sf::float64_t (max)        () noexcept { return sf::float64_t::my_value_max(); }
    static constexpr sf::float64_t lowest       () noexcept { return sf::float64_t::my_value_lowest(); }
    static constexpr sf::float64_t epsilon      () noexcept { return sf::float64_t::my_value_epsilon(); }
    static constexpr sf::float64_t round_error  () noexcept { return sf::float64_t::my_value_round_error(); }
    static constexpr sf::float64_t denorm_min   () noexcept { return sf::float64_t::my_value_denorm_min(); }
    static constexpr sf::float64_t infinity     () noexcept { return sf::float64_t::my_value_infinity(); }
    static constexpr sf::float64_t quiet_NaN    () noexcept { return sf::float64_t::my_value_quiet_NaN(); }
    static constexpr sf::float64_t signaling_NaN() noexcept { return sf::float64_t::my_value_signaling_NaN(); }
  };

  }

#endif // SOFT_DOUBLE_2020_10_27_H_
