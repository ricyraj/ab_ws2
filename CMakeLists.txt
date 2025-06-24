cmake_minimum_required(VERSION 3.10)
project(WsRtd)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)

add_library(WsRtd SHARED
    src/WsRtd.cpp
)

target_include_directories(WsRtd PRIVATE include)
