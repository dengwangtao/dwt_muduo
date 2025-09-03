
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog)

# aux_source_directory(. SRC_LIST)

# 扫描所有c++文件
file(GLOB_RECURSE SRC_HEADER CONFIGURE_DEPENDS *.h)
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS *.cpp *.cc)


set(TARGET dwt_muduo)
add_library(${TARGET} STATIC ${SRC_LIST})



set(FMT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/fmt)
set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/thirdparty/spdlog)



# target_link_libraries(${TARGET} pthread)

# 这行让其他目录能通过 `#include "hello/xxx.h"` 访问
target_include_directories(${TARGET} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FMT_DIR}/include
    ${SPDLOG_DIR}/include
)

target_link_directories(${TARGET} PUBLIC
    ${FMT_DIR}/lib
)

target_link_libraries(${TARGET} PUBLIC
    fmt
)
