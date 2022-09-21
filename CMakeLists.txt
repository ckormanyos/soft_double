project(soft-float)

cmake_minimum_required(VERSION 3.16.3)

include(CTest)

add_subdirectory("examples")
add_subdirectory("test")

add_library(SoftFloat INTERFACE)
target_compile_features(SoftFloat INTERFACE cxx_std_11)

target_include_directories(
  SoftFloat SYSTEM INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

install(TARGETS SoftFloat EXPORT SoftFloatTargets)
install(
  FILES math/softfloat/soft_double.h
  DESTINATION include/math/softfloat/)
install(EXPORT SoftFloatTargets
  FILE SoftFloatConfig.cmake
  NAMESPACE SoftFloat::
  DESTINATION lib/cmake/soft-float)
