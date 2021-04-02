# soft_double
soft_double provides a C++ software implementation of a double-precision floating-point data type

---

This work is in a preliminary state. Changes are planned including:
  - Better namespace names for the `soft_double` type.
  - Specialize further the template `std::numeric_limits<soft_double>`.
  - Consider header-only design.
  - Some `constexpr` sequences an't be resolved in C++11 and need C++14. Strive for 11 through 20 compatibility.
  - Add more tests and depth of CI.

---
