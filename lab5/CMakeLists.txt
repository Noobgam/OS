cmake_minimum_required(VERSION 2.8)

project(OS5)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-exceptions -std=c++11 -pedantic -fexceptions")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fexceptions -g -fsanitize=address,undefined")
endif()

add_library(calculator calculator.cpp)
add_library(calculator_shared SHARED calculator_shared.cpp)

add_executable(main main.cpp)

target_link_libraries(main -lpthread calculator)

add_executable(maindyn maindyn.cpp)

target_link_libraries(maindyn -ldl)
