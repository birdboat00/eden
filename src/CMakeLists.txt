cmake_minimum_required(VERSION 3.23)
project(eden)

add_subdirectory(libeden)
add_subdirectory(libeden_niftest)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_executable(eden main.cc)
target_link_libraries(eden libeden)

set_property(TARGET eden PROPERTY CXX_STANDARD 20)