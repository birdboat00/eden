cmake_minimum_required(VERSION 3.20)
project(libeden)

set(SOURCE_FILES
  bc.cc
  pack.cc
  term.cc
)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_library(libeden ${SOURCE_FILES})
set_property(TARGET libeden PROPERTY CXX_STANDARD 20)