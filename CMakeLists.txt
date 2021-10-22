cmake_minimum_required(VERSION 3.20)
project(simple_db)

set(CMAKE_CXX_STANDARD 20)

add_executable(simple_db
        main.cpp)
target_compile_options(simple_db PUBLIC -g)
