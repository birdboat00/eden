cmake_minimum_required(VERSION 3.23)

project(eden VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

add_library(libeden
  src/libeden/bc.cc
  src/libeden/bif.cc
  src/libeden/builtin_test_pack.cc
  src/libeden/err.cc
  src/libeden/pack.cc
  src/libeden/term.cc
  src/libeden/vm.cc
)

add_executable(edencc src/main.cc)

target_link_libraries(edencc PRIVATE libeden)