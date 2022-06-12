cmake_minimum_required(VERSION 3.23)

project(
  eden
  VERSION 0.0.1
  LANGUAGES C
)

add_executable(eden
  src/bif.c
  src/builtin_test_pack.c
  src/error.c
  src/main.c
  src/op.c
  src/pack.c
  src/term.c
  src/vm.c
)