cmake_minimum_required(VERSION 3.27)
project(local_socket)

set(CMAKE_CXX_STANDARD 14)

add_executable(local_socket
        server.cpp)

add_executable(client
        client.cpp)
