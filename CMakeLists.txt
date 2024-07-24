cmake_minimum_required(VERSION 3.29)
project(sdlgol C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(gol main.c)
target_link_libraries(gol ${SDL2_LIBRARIES})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Ofast -Wall -Wextra -Wpedantic")