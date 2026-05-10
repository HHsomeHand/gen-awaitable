cmake_minimum_required(VERSION 3.10)
project(N01_gen_awaitable)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(
            /source-charset:utf-8
            /execution-charset:utf-8
    )
endif()

add_executable(N01_gen_awaitable main.cpp
        generator.cpp
        generator.hpp
        task.cpp
        task.hpp
        timer.cpp
        timer.hpp)