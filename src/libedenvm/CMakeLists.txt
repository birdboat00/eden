cmake_minimum_required(VERSION 3.20)
project(libedenvm)

set(SOURCE_FILES
  nif.cc
  sched.cc
  vm.cc
)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()


add_library(libedenvm STATIC ${SOURCE_FILES})

target_include_directories(libedenvm PUBLIC "../")

target_link_libraries(libedenvm libeden)

set_property(TARGET libedenvm PROPERTY CXX_STANDARD 20)