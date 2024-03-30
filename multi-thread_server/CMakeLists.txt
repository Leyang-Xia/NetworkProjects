cmake_minimum_required(VERSION 3.27)
project(multi_thread_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(multi_thread_server
        server.cpp)
