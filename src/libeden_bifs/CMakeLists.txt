cmake_minimum_required(VERSION 3.20)
project(libeden_bifs)

set(SOURCE_FILES
  edn_bifs.cc
  arith.cc
  dbg.cc
  pack.cc
  rt.cc
  term.cc
)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_library(libeden_bifs STATIC ${SOURCE_FILES})

target_include_directories(libeden_bifs PRIVATE "../")

target_link_libraries(libeden_bifs libeden)
target_link_libraries(libeden_bifs libedenvm)

set_property(TARGET libeden_bifs PROPERTY CXX_STANDARD 20)