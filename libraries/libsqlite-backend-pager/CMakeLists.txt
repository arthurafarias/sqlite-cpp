cmake_minimum_required(VERSION 3.24)
project(sqlite-backend-pager LANGUAGES C)

# SRS 001 FR-4: see cmake/SqliteCppLibrary.cmake for what this does and why
# no sibling sqlite-cpp library is linked against here.
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppLibrary.cmake)
sqlite_cpp_add_library(backend-pager)
