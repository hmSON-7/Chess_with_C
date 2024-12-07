cmake_minimum_required(VERSION 3.29)
project(Chess_with_C C)

set(CMAKE_C_STANDARD 99)

add_executable(Chess_with_C chess/main.c
        chess/event/Check.c
        chess/event/event.c)
