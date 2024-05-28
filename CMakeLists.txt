cmake_minimum_required(VERSION 3.0)

project(dwt_muduo)

# 库生成路径
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)