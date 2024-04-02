cmake_minimum_required(VERSION 3.27)
project(thread_pool)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(thread_pool
        threadpool.cpp
        threadpool.h
        main.cpp)
