cmake_minimum_required(VERSION 3.29)
project(kris C)

set(CMAKE_C_STANDARD 99)

add_executable(kris
    main.c
        historicList.c
        historicList.h
        fileList.c
        fileList.h)
