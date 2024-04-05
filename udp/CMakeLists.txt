cmake_minimum_required(VERSION 3.27)
project(udp)

set(CMAKE_CXX_STANDARD 14)

add_executable(udp_server
        server.cpp)
add_executable(udp_client client.cpp)
