cmake_minimum_required(VERSION 3.27)
project(libevent)

set(CMAKE_CXX_STANDARD 14)

LINK_DIRECTORIES(/usr/local/lib)

add_executable(libevent
        event.cpp)

TARGET_LINK_LIBRARIES(libevent event)
