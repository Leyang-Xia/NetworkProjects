cmake_minimum_required(VERSION 3.27)
project(cs_mod_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(client client.cpp)

add_executable(server server.cpp)
