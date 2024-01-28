# Copyright 2024 isaki

cmake_minimum_required(VERSION 3.28)

project(
    Menphina
    VERSION 0.0.1
    LANGUAGES CXX)

# Json Library; used by all build targets.
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Configure C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# Compiler flags
add_compile_options(-O2 -pipe -Wall -Wextra -Wpedantic -Werror)

# Configure CMAKE
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Add the directories
add_subdirectory(configured_files)
add_subdirectory(src)
