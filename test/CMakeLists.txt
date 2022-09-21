enable_testing()
find_package(Threads)

add_executable(test_soft_double
  test.cpp
  test_soft_double.cpp
  test_soft_double_examples.cpp
  test_soft_double_spot_values.cpp)
target_compile_features(test_soft_double PRIVATE cxx_std_11)
target_include_directories(test_soft_double PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(test_soft_double Examples ${CMAKE_THREAD_LIBS_INIT})
add_test(test test_soft_double)
