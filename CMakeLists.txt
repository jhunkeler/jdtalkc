cmake_minimum_required(VERSION 2.8)
project(jdtalkc C)

set(CMAKE_C_STANDARD 99)

add_executable(jdtalkc dictionary.c strings.c talk.c main.c jdtalk.h)
