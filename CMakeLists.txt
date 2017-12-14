cmake_minimum_required(VERSION 3.0.0)
project(template_magic VERSION 0.0.0)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(int_sort static-int-sort.cpp)
add_executable(types_sort static-sort.cpp)
add_executable(topological_sort static-topological-sort.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})