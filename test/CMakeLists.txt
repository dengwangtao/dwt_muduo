

include_directories(${PROJECT_SOURCE_DIR}/src)

aux_source_directory(. SRC_LIST)

add_executable(test ${SRC_LIST})

target_link_libraries(test dwt_muduo pthread)