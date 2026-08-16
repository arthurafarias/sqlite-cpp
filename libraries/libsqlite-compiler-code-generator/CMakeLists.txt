cmake_minimum_required(VERSION 3.24)
project(sqlite-compiler-code-generator LANGUAGES C)

# SRS 001 FR-4: see cmake/SqliteCppLibrary.cmake for what this does and why
# no sibling sqlite-cpp library is linked against here.

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/csrc/*.c)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
