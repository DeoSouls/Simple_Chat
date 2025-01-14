cmake_minimum_required(VERSION 3.16)
project(ChatServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core WebSockets Sql)

add_executable(ChatServer
    main_server.cpp
    src/chatserver.cpp
    src/chatserver.h
)

target_link_libraries(ChatServer PRIVATE
    Qt6::Core
    Qt6::WebSockets
    Qt6::Sql
)