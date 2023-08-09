cmake_minimum_required(VERSION 3.25)

project(cs2-sdk)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(WIN32_LEAN_AND_MEAN)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/game.cpp
    src/hooks/create_move.cpp
    src/hooks/frame_stage_notify.cpp
    src/hooks/hooks.cpp
    src/hooks/level_init.cpp
    src/interfaces.cpp
    src/utility/memory.cpp
    src/utility/pe.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)