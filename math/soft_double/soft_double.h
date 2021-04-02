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

  #include <softfloat.h>

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
    constexpr soft_double(const std::int8_t  n) : my_value(::my__i32_to_f64((std::int32_t) n)) { }
    constexpr soft_double(const std::int16_t n) : my_value(::my__i32_to_f64((std::int32_t) n)) { }
    constexpr soft_double(const std::int32_t n) : my_value(::my__i32_to_f64(               n)) { }
    constexpr soft_double(const std::int64_t n) : my_value(::my__i64_to_f64(               n)) { }

    constexpr soft_double(const std::uint8_t  u) : my_value(::my_ui32_to_f64((std::uint32_t) u)) { }
    constexpr soft_double(const std::uint16_t u) : my_value(::my_ui32_to_f64((std::uint32_t) u)) { }
    constexpr soft_double(const std::uint32_t u) : my_value(::my_ui32_to_f64(                u)) { }
    constexpr soft_double(const std::uint64_t u) : my_value(::my_ui64_to_f64(                u)) { }

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

    friend bool isfinite(const soft_double x) { return ((isnan(x) == false) && (isinf(x) == false)); }
    friend bool isnan   (const soft_double x) { return  (x.my_value == my_value_quiet_NaN().my_value); }
    friend bool isinf   (const soft_double x) { return ((x.my_value & my_value_infinity().my_value) == my_value_infinity().my_value); }

    friend inline soft_double fabs(const soft_double x) { return soft_double((((int64_t) x.my_value < 0) ? (uint64_t) (-((int64_t) x.my_value)) : x.my_value), detail::nothing()); }
    friend inline soft_double sqrt(const soft_double x) { return soft_double(::f64_sqrt(x.my_value), detail::nothing()); }

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
