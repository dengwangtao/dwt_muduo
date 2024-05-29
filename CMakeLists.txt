cmake_minimum_required(VERSION 3.15)

project(dwt_muduo)

# 库生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)

add_subdirectory(test)