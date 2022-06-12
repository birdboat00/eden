cmake_minimum_required(VERSION 3.23)

project(
  eden
  VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 20)

add_library(libceden
  src/libceden/bif.c
  src/libceden/builtin_test_pack.c
  src/libceden/error.c
  src/libceden/op.c
  src/libceden/pack.c
  src/libceden/term.c
  src/libceden/vm.c
)

add_library(libeden
  src/libeden/bc.cc
  src/libeden/err.cc
  src/libeden/pack.cc
  src/libeden/term.cc
)

add_executable(eden src/main.c)
add_executable(edencc src/main.cc)

target_link_libraries(eden PRIVATE libceden)
target_link_libraries(edencc PRIVATE libeden)