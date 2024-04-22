cmake_minimum_required(VERSION 3.27)
project(buffer_event)

set(CMAKE_CXX_STANDARD 14)

# 告诉编译器在链接时在指定目录查找库文件
LINK_DIRECTORIES(/usr/local/lib)

add_executable(buffer_event
        buffer.cpp)

add_executable(client client.cpp)

TARGET_LINK_LIBRARIES(client event)
TARGET_LINK_LIBRARIES(buffer_event event)
