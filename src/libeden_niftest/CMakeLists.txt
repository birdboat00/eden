cmake_minimum_required(VERSION 3.20)
project(libeden_niftest)

set(SOURCE_FILES
  eden_niftest.cc
)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_library(libeden_niftest SHARED ${SOURCE_FILES})

target_include_directories(libeden_niftest PRIVATE "../libeden/")

target_link_libraries(libeden_niftest libeden)
target_link_libraries(libeden_niftest libedenvm)

set_property(TARGET libeden_niftest PROPERTY CXX_STANDARD 20)