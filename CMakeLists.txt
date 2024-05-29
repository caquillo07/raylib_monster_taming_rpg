cmake_minimum_required(VERSION 3.27)
project(raylib_pokemon_clone C)

set(CMAKE_C_STANDARD 23)
set(PROJECT_NAME raylib_pokemon_clone)

include(FetchContent)

file(GLOB SOURCES *.c *.cpp *.h *.hpp src/*.c src/*.cpp src/*.h src/*.hpp src/*/*.c src/*/*.cpp src/*/*.h src/*/*.hpp)
add_executable(${PROJECT_NAME} ${SOURCES})

# FetchContent deps
FetchContent_Declare(
        tmx
        GIT_REPOSITORY https://github.com/baylej/tmx
        GIT_TAG tmx_1.10.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tmx)
#target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE )
target_link_libraries(${PROJECT_NAME} PRIVATE tmx)

# Manually vendored deps
add_subdirectory(vendors/slog)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/vendors/slog/src)
target_link_libraries(${PROJECT_NAME} PRIVATE slog)

# because of raylib - TODO(hector) - make this only for debug builds, figure out
#  why this is even needed.
if(APPLE)
    message("Building for macOS")
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O1 -fno-omit-frame-pointer -Wall -Wextra -fsanitize=address)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
elseif (UNIX AND NOT APPLE)
    message("Building for Linux")
    target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -fno-omit-frame-pointer -Wall -Wextra -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME} PRIVATE -g -fsanitize=address,undefined)
endif ()

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
