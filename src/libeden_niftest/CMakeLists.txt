cmake_minimum_required(VERSION 3.23)
project(libeden_niftest)

set(SOURCE_FILES
  eden_niftest.cc
)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_library(libeden_niftest SHARED ${SOURCE_FILES})
target_link_libraries(libeden_niftest libeden)
set_property(TARGET libeden_niftest PROPERTY CXX_STANDARD 20)