cmake_minimum_required(VERSION 3.10)

project(demo12 C)
add_executable(demo12 main.c)
target_link_libraries(demo12 pthread rt)

