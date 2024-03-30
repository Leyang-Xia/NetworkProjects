cmake_minimum_required(VERSION 3.27)
project(multi_process_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(multi_process_server server.cpp)
add_executable(client client.cpp)
