add_library(Examples
  example001_roots_sqrt.cpp
  example004_bessel_recur.cpp
  example005_polylog_series.cpp
  example007_catalan_series.cpp
  example010_hypergeometric_2f1.cpp
  example011_trig_trapezoid_integral.cpp
  example012_exercise_constexpr.cpp)
target_compile_features(Examples PRIVATE cxx_std_11)
target_include_directories(Examples PRIVATE ${PROJECT_SOURCE_DIR})
