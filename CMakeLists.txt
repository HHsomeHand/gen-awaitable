cmake_minimum_required(VERSION 4.2)
project(N01_gen_awaitable)

set(CMAKE_CXX_STANDARD 20)

add_executable(N01_gen_awaitable main.cpp
        generator.cpp
        generator.hpp
        task.cpp
        task.hpp
        timer.cpp
        timer.hpp)
