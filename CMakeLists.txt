cmake_minimum_required(VERSION 3.27)
project(raylib_pokemon_clone C)

set(CMAKE_C_STANDARD 23)
set(PROJECT_NAME raylib_pokemon_clone)

file(GLOB SOURCES *.c *.cpp *.h *.hpp src/*.c src/*.cpp src/*.h src/*.hpp src/*/*.c src/*/*.cpp src/*/*.h src/*/*.hpp)
add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(vendors/slog)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/vendors/slog/src)
target_link_libraries(${PROJECT_NAME} PRIVATE slog)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/vendors/cute_headers)
#target_link_libraries(${PROJECT_NAME} PRIVATE cute_headers)

# because of raylib - TODO(hector) - make this only for debug builds, figure out
#  why this is even needed.
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -fsanitize=address)
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
