cmake_minimum_required(VERSION 4.0.2)
project(now.c VERSION 0.1.0 LANGUAGES C)

find_package(raylib CONFIG REQUIRED)

add_executable(now.c main.c)

target_link_libraries(now.c PRIVATE raylib)

