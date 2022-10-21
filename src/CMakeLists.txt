cmake_minimum_required(VERSION 3.20)
project(eden)

add_subdirectory(libeden)
add_subdirectory(libedenvm)
add_subdirectory(libeden_bifs)
add_subdirectory(libeden_niftest)
add_subdirectory(libedenasm)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -ggdb")
endif()

add_executable(eden main.cc builtin_test_pack.cc)

target_include_directories(eden PRIVATE "./")

target_link_libraries(eden libeden)
target_link_libraries(eden libedenvm)
target_link_libraries(eden libeden_bifs)
target_link_libraries(eden libedenasm)

set_property(TARGET eden PROPERTY CXX_STANDARD 20)