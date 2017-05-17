cmake_minimum_required(VERSION 3.7)
project(a3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        inc/SOIL/SOIL.h
        src/camera.c
        src/camera.h
        src/controls.h
        src/gl.h
        src/level.c
        src/level.h
        src/main.c
        src/material.c
        src/material.h
        src/mesh.c
        src/mesh.h
        src/osd.c
        src/osd.h
        src/player.c
        src/player.h
        src/state.h
        src/util.c
        src/util.h
        src/vec.c
        src/vec.h)

add_executable(a3 ${SOURCE_FILES})