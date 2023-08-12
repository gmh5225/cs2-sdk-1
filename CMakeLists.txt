cmake_minimum_required(VERSION 3.25)

project(cs2-sdk)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(WIN32_LEAN_AND_MEAN)

find_package(spdlog CONFIG REQUIRED)

find_path(XORSTR_INCLUDE_DIRS "xorstr.hpp")

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/game.cpp
    src/hooks/create_move.cpp
    src/hooks/frame_stage_notify.cpp
    src/hooks/hooks.cpp
    src/hooks/level_init.cpp
    src/hooks/present.cpp
    src/hooks/resize_buffers.cpp
    src/interfaces.cpp
    src/schemas.cpp
    src/utility/memory.cpp
    src/utility/pe.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)

target_include_directories(${PROJECT_NAME} PRIVATE ${XORSTR_INCLUDE_DIRS} include)