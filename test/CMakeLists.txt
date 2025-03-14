# 包含external头文件路径
include_directories(${PROJECT_SOURCE_DIR}/external/GmSSL/include)    # GmSSL

# 设置编译选项以启用调试信息
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 创建可执行文件目标
add_executable(sm2 sm2.c)

# 链接库到可执行文件
target_link_libraries(sm2 PRIVATE m pthread gmssl)