cmake_minimum_required(VERSION 3.20)
project(libedenasm)

set(SOURCE_FILES
  asm.cc
)
set(HEADER_FILES
  asm.hh
)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_library(libedenasm STATIC ${SOURCE_FILES})

target_include_directories(libedenasm PRIVATE "../")

target_link_libraries(libedenasm libeden)

set_property(TARGET libedenasm PROPERTY CXX_STANDARD 20)