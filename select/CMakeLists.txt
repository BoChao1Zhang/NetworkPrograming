cmake_minimum_required(VERSION 3.15)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/utility)

# 添加源文件
set(SERVER_SOURCES
    src/server.cpp
    src/socket.cpp
    src/server_socket.cpp
    utility/logger.cpp
)

set(SELECT_SERVER_SOURCES
    src/socket.cpp
    src/server_socket.cpp
    utility/logger.cpp
    src/select.cpp
    src/selector.cpp
    src/select_handle.cpp
)

set(CLIENT_SOURCES
    src/client.cpp
    src/socket.cpp
    src/client_socket.cpp
    utility/logger.cpp
)

# 生成可执行文件
# add_executable(server ${SERVER_SOURCES})
add_executable(select_server ${SELECT_SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

# 链接库（如果有需要的话，可以在这里添加库链接）
# target_link_libraries(server ...)
# target_link_libraries(client ...)