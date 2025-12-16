cmake_minimum_required(VERSION 3.10)

project(demo10 C)
add_executable(demo10 main.c)
target_link_libraries(demo10 pthread)

