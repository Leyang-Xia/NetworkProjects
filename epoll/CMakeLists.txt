cmake_minimum_required(VERSION 3.27)
project(epoll)

set(CMAKE_CXX_STANDARD 14)

add_executable(epoll
        server.cpp)
