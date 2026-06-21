cmake_minimum_required(VERSION 3.20)

project(pong LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

add_executable(server
        include/server.h
        src/server.cpp
        main_server.cpp
        include/utils.h
        include/network.hpp
)

target_include_directories(server PRIVATE include)

target_link_libraries(server
        PRIVATE
        Boost::system
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
        ws2_32
)

add_executable(client
        include/client.h
        src/client.cpp
        main_client.cpp
        include/utils.h
        include/renderer.h
        include/network.hpp
        src/renderer.cpp
        assets/arial.ttf
)

target_include_directories(client PRIVATE include)

target_link_libraries(client
        PRIVATE
        Boost::system
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
        ws2_32
)