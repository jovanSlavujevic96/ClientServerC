project(ClientServer)
cmake_minimum_required(VERSION 3.18)

find_package(Threads REQUIRED)

add_executable(client Client.c)

add_executable(server Server.c)
target_link_libraries(server pthread)