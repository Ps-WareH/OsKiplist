cmake_minimum_required(VERSION 3.26)
project(OsKiplist)

set(CMAKE_CXX_STANDARD 17)
include_directories(${PROJECT_SOURCE_DIR}/include)
# 将 main.cpp 和其他源文件一起编译为可执行文件
add_executable(OsKiplist main.cpp
        lib/HttpServer.cpp
        lib/OsKiplist.cpp)

# 链接 pthread 库，确保支持多线程
target_link_libraries(OsKiplist pthread)

