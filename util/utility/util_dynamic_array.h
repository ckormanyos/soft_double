﻿///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2012 - 2022.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef UTIL_DYNAMIC_ARRAY_2012_05_16_H // NOLINT(llvm-header-guard)
  #define UTIL_DYNAMIC_ARRAY_2012_05_16_H

  #include <cstddef>
  #include <cstdint>
  #include <initializer_list>
  #include <iterator>
  #include <memory>
  #include <utility>

  #include <util/utility/util_constexpr_algorithm_unsafe.h>

  #if defined(_MSVC_LANG)
    #if (_MSVC_LANG >= 202002L)
    #define DYNAMIC_ARRAY_CONSTEXPR constexpr               // NOLINT(cppcoreguidelines-macro-usage)
    #define DYNAMIC_ARRAY_CONSTEXPR_IS_COMPILE_TIME_CONST 1 // NOLINT(cppcoreguidelines-macro-usage)
    #else
    #define DYNAMIC_ARRAY_CONSTEXPR                         // NOLINT(cppcoreguidelines-macro-usage)
    #define DYNAMIC_ARRAY_CONSTEXPR_IS_COMPILE_TIME_CONST 0 // NOLINT(cppcoreguidelines-macro-usage)
    #endif
  #else
    #if (__cplusplus >= 202002L)
    #define DYNAMIC_ARRAY_CONSTEXPR constexpr               // NOLINT(cppcoreguidelines-macro-usage)
    #define DYNAMIC_ARRAY_CONSTEXPR_IS_COMPILE_TIME_CONST 1 // NOLINT(cppcoreguidelines-macro-usage)
    #else
    #define DYNAMIC_ARRAY_CONSTEXPR                         // NOLINT(cppcoreguidelines-macro-usage)
    #define DYNAMIC_ARRAY_CONSTEXPR_IS_COMPILE_TIME_CONST 0 // NOLINT(cppcoreguidelines-macro-usage)
    #endif
  #endif

  namespace util {

  template<typename ValueType,
           typename AllocatorType = std::allocator<ValueType>,
           typename SizeType      = std::size_t,
           typename DiffType      = std::ptrdiff_t>
  class dynamic_array;

  template<typename ValueType,
           typename AllocatorType,
           typename SizeType,
           typename DiffType>
  class dynamic_array
  {
  public:
    // Type definitions.
    using allocator_type         = typename std::allocator_traits<AllocatorType>::template rebind_alloc<ValueType>;
    using value_type             = typename allocator_type::value_type;
    using reference              =       value_type&;
    using const_reference        = const value_type&;
    using iterator               =       value_type*;
    using const_iterator         = const value_type*;
    using pointer                =       value_type*;
    using const_pointer          = const value_type*;
    using size_type              =       SizeType;
    using difference_type        =       DiffType;
    using reverse_iterator       =       std::reverse_iterator<iterator>;
    using const_reverse_iterator =       std::reverse_iterator<const_iterator>;

    // Constructors.
    explicit constexpr dynamic_array(      size_type       count = static_cast<size_type>(UINT8_C(0)),
                                           const_reference v     = value_type(),
                                     const allocator_type& a     = allocator_type())
      : elem_count(count)
    {
      if(elem_count > static_cast<size_type>(UINT8_C(0)))
      {
        allocator_type my_a(a);

        elems = std::allocator_traits<allocator_type>::allocate(my_a, elem_count);

        iterator it = begin();

        while(it != end())
        {
          std::allocator_traits<allocator_type>::construct(my_a, it, v);

          ++it;
        }
      }
    }

    constexpr dynamic_array(const dynamic_array& other)
      : elem_count(other.size())
    {
      allocator_type my_a;

      if(elem_count > static_cast<size_type>(UINT8_C(0)))
      {
        elems = std::allocator_traits<allocator_type>::allocate(my_a, elem_count);
      }

      util::copy_unsafe(other.elems, other.elems + elem_count, elems);
    }

    template<typename input_iterator>
    constexpr dynamic_array(input_iterator first,
                            input_iterator last,
                            const allocator_type& a = allocator_type())
      : elem_count(static_cast<size_type>(last - first))
    {
      // TBD: Ensure that the type of input_iterator supports subtract for distance.

      allocator_type my_a(a);

      if(elem_count > static_cast<size_type>(UINT8_C(0)))
      {
        elems = std::allocator_traits<allocator_type>::allocate(my_a, elem_count);
      }

      util::copy_unsafe(first, last, elems);
    }

    constexpr dynamic_array(std::initializer_list<value_type> lst,
                            const allocator_type& a = allocator_type())
      : elem_count(lst.size())
    {
      allocator_type my_a(a);

      if(elem_count > static_cast<size_type>(UINT8_C(0)))
      {
        elems = std::allocator_traits<allocator_type>::allocate(my_a, elem_count);
      }

      util::copy_unsafe(lst.begin(), lst.end(), elems);
    }

    // Move constructor.
    constexpr dynamic_array(dynamic_array&& other) noexcept : elem_count(other.elem_count),
                                                              elems     (other.elems)
    {
      other.elem_count = static_cast<size_type>(UINT8_C(0));
      other.elems      = nullptr;
    }

    // Destructor.
    DYNAMIC_ARRAY_CONSTEXPR
    virtual ~dynamic_array() // LCOV_EXCL_LINE
    {
      using local_allocator_traits_type = std::allocator_traits<allocator_type>;

      allocator_type my_a;

      // Destroy the elements and deallocate the range.
      local_allocator_traits_type::deallocate(my_a, elems, elem_count);
    }

    // Assignment operator.
    constexpr auto operator=(const dynamic_array& other) -> dynamic_array&
    {
      if(this != &other)
      {
        util::copy_unsafe(other.elems,
                          other.elems + util::min_unsafe(elem_count, other.elem_count),
                          elems);
      }

      return *this;
    }

    // Move assignment operator.
    constexpr auto operator=(dynamic_array&& other) noexcept -> dynamic_array&
    {
      util::swap_unsafe(elem_count, other.elem_count);
      util::swap_unsafe(elems,      other.elems);

      return *this;
    }

    // Iterator members:
    constexpr auto begin  ()       noexcept -> iterator               { return elems; }                                      // NOLINT(modernize-use-nodiscard)
    constexpr auto end    ()       noexcept -> iterator               { return elems + elem_count; }                         // NOLINT(modernize-use-nodiscard)
    constexpr auto begin  () const noexcept -> const_iterator         { return elems; }                                      // NOLINT(modernize-use-nodiscard)
    constexpr auto end    () const noexcept -> const_iterator         { return elems + elem_count; }                         // NOLINT(modernize-use-nodiscard)
    constexpr auto cbegin () const noexcept -> const_iterator         { return elems; }                                      // NOLINT(modernize-use-nodiscard)
    constexpr auto cend   () const noexcept -> const_iterator         { return elems + elem_count; }                         // NOLINT(modernize-use-nodiscard)
    constexpr auto rbegin ()       noexcept -> reverse_iterator       { return reverse_iterator(elems + elem_count); }       // NOLINT(modernize-use-nodiscard)
    constexpr auto rend   ()       noexcept -> reverse_iterator       { return reverse_iterator(elems); }                    // NOLINT(modernize-use-nodiscard)
    constexpr auto rbegin () const noexcept -> const_reverse_iterator { return const_reverse_iterator(elems + elem_count); } // NOLINT(modernize-use-nodiscard)
    constexpr auto rend   () const noexcept -> const_reverse_iterator { return const_reverse_iterator(elems); }              // NOLINT(modernize-use-nodiscard)
    constexpr auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(elems + elem_count); } // NOLINT(modernize-use-nodiscard)
    constexpr auto crend  () const noexcept -> const_reverse_iterator { return const_reverse_iterator(elems); }              // NOLINT(modernize-use-nodiscard)

    // Raw pointer access.
    constexpr auto data()       noexcept -> pointer       { return elems; } // NOLINT(modernize-use-nodiscard)
    constexpr auto data() const noexcept -> const_pointer { return elems; } // NOLINT(modernize-use-nodiscard)

    // Size and capacity.
    constexpr auto size    () const noexcept -> size_type { return  elem_count; } // NOLINT(modernize-use-nodiscard)
    constexpr auto max_size() const noexcept -> size_type { return  elem_count; } // NOLINT(modernize-use-nodiscard)
    constexpr auto empty   () const noexcept -> bool      { return (elem_count == static_cast<size_type>(UINT8_C(0))); } // NOLINT(modernize-use-nodiscard)

    // Element access members.
    constexpr auto operator[](const size_type i)       noexcept -> reference       { return elems[i]; } // NOLINT(modernize-use-nodiscard)
    constexpr auto operator[](const size_type i) const noexcept -> const_reference { return elems[i]; } // NOLINT(modernize-use-nodiscard)

    constexpr auto front()       noexcept -> reference       { return elems[static_cast<size_type>(UINT8_C(0))]; } // NOLINT(modernize-use-nodiscard)
    constexpr auto front() const noexcept -> const_reference { return elems[static_cast<size_type>(UINT8_C(0))]; } // NOLINT(modernize-use-nodiscard)

    constexpr auto back()       noexcept -> reference       { return ((elem_count > static_cast<size_type>(UINT8_C(0))) ? elems[static_cast<size_type>(elem_count - static_cast<size_type>(UINT8_C(1)))] : elems[static_cast<size_type>(UINT8_C(0))]); } // NOLINT(modernize-use-nodiscard)
    constexpr auto back() const noexcept -> const_reference { return ((elem_count > static_cast<size_type>(UINT8_C(0))) ? elems[static_cast<size_type>(elem_count - static_cast<size_type>(UINT8_C(1)))] : elems[static_cast<size_type>(UINT8_C(0))]); } // NOLINT(modernize-use-nodiscard)

    constexpr auto at(const size_type i)       noexcept -> reference       { return ((i < elem_count) ? elems[i] : elems[static_cast<size_type>(UINT8_C(0))]); } // NOLINT(modernize-use-nodiscard)
    constexpr auto at(const size_type i) const noexcept -> const_reference { return ((i < elem_count) ? elems[i] : elems[static_cast<size_type>(UINT8_C(0))]); } // NOLINT(modernize-use-nodiscard)

    // Element manipulation members.
    constexpr auto fill(const value_type& v) -> void
    {
      util::fill_unsafe(begin(), begin() + elem_count, v);
    }

    constexpr auto swap(dynamic_array& other) noexcept -> void
    {
      if(this != &other)
      {
        util::swap_unsafe(elems,      other.elems);
        util::swap_unsafe(elem_count, other.elem_count);
      }
    }

    constexpr auto swap(dynamic_array&& other) noexcept -> void
    {
      elems      = std::move(other.elems);
      elem_count = std::move(other.elem_count);
    }

  private:
    size_type elem_count;        // NOLINT(readability-identifier-naming)
    pointer   elems { nullptr }; // NOLINT(readability-identifier-naming,altera-id-dependent-backward-branch)
  };

  template<typename ValueType, typename AllocatorType>
  auto operator==(const dynamic_array<ValueType, AllocatorType>& lhs,
                  const dynamic_array<ValueType, AllocatorType>& rhs) -> bool
  {
    bool left_and_right_are_equal = false;

    if(lhs.size() == rhs.size())
    {
      using size_type = typename dynamic_array<ValueType, AllocatorType>::size_type;

      const auto size_of_left_is_zero = (lhs.size() == static_cast<size_type>(UINT8_C(0)));

      left_and_right_are_equal =
        (size_of_left_is_zero || std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin()));
    }

    return left_and_right_are_equal;
  }

  template<typename ValueType, typename AllocatorType>
  auto operator<(const dynamic_array<ValueType, AllocatorType>& lhs,
                 const dynamic_array<ValueType, AllocatorType>& rhs) -> bool
  {
    using size_type = typename dynamic_array<ValueType, AllocatorType>::size_type;

    const auto size_of_left_is_zero = (lhs.size() == static_cast<size_type>(UINT8_C(0)));

    bool b_result { };

    if(size_of_left_is_zero)
    {
      const auto size_of_right_is_zero = (rhs.size() == static_cast<size_type>(UINT8_C(0)));

      b_result = (!size_of_right_is_zero);
    }
    else
    {
      if(size_of_left_is_zero)
      {
        const auto size_of_right_is_zero = (rhs.size() == static_cast<size_type>(UINT8_C(0)));

        b_result = (!size_of_right_is_zero);
      }
      else
      {
        const size_type count = util::min_unsafe(lhs.size(), rhs.size());

        b_result= std::lexicographical_compare(lhs.cbegin(),
                                               lhs.cbegin() + count,
                                               rhs.cbegin(),
                                               rhs.cbegin() + count);
      }
    }

    return b_result;
  }

  template<typename ValueType, typename AllocatorType>
  auto operator!=(const dynamic_array<ValueType, AllocatorType>& lhs,
                  const dynamic_array<ValueType, AllocatorType>& rhs) -> bool
  {
    return (!(lhs == rhs));
  }

  template<typename ValueType, typename AllocatorType>
  auto operator>(const dynamic_array<ValueType, AllocatorType>& lhs,
                 const dynamic_array<ValueType, AllocatorType>& rhs) -> bool
  {
    return (rhs < lhs);
  }

  template<typename ValueType, typename AllocatorType>
  auto operator>=(const dynamic_array<ValueType, AllocatorType>& lhs,
                  const dynamic_array<ValueType, AllocatorType>& rhs) -> bool
  {
    return (!(lhs < rhs));
  }

  template<typename ValueType, typename AllocatorType>
  auto operator<=(const dynamic_array<ValueType, AllocatorType>& lhs,
                  const dynamic_array<ValueType, AllocatorType>& rhs) -> bool
  {
    return (!(rhs < lhs));
  }

  template<typename ValueType, typename AllocatorType>
  auto swap(dynamic_array<ValueType, AllocatorType>& x,
            dynamic_array<ValueType, AllocatorType>& y) noexcept -> void
  {
    x.swap(y);
  }

  } // namespace util

#endif // UTIL_DYNAMIC_ARRAY_2012_05_16_H
