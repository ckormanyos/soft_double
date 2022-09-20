project(soft-float)

cmake_minimum_required(VERSION 3.16.3)

include(CTest)

add_subdirectory("examples")
add_subdirectory("test")

add_library(SoftDouble INTERFACE)
target_compile_features(SoftDouble INTERFACE cxx_std_11)

target_include_directories(
  SoftDouble SYSTEM INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

install(TARGETS SoftDouble EXPORT SoftDoubleTargets)
install(
  FILES math/softfloat/soft_double.h
  DESTINATION include/math/softfloat/)
install(EXPORT SoftFloatTargets
  FILE SoftFloatConfig.cmake
  NAMESPACE SoftFloat::
  DESTINATION lib/cmake/soft-float)
