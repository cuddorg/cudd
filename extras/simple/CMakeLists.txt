cmake_minimum_required(VERSION 3.10)

project(cudd_simple)

find_package(cudd 4.0.0 REQUIRED)

add_executable(cudd_simple_main main.c)
target_link_libraries(cudd_simple_main PRIVATE cudd::cudd)


