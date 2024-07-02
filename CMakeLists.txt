cmake_minimum_required(VERSION 3.10)

project(ChatServer)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(server main/server_main.cpp src/server.cpp)
add_executable(client main/client_main.cpp src/client.cpp)

target_link_libraries(server pthread)
target_link_libraries(client pthread)
