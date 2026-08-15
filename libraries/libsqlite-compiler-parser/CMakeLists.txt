cmake_minimum_required(VERSION 3.24)
project(sqlite-compiler-parser LANGUAGES C)

# SRS 001 FR-4: see cmake/SqliteCppLibrary.cmake for what this does and why
# no sibling sqlite-cpp library is linked against here. csrc/ holds parse.y
# (the grammar source, per SRS 001 FR-3) -- the generated parse.c
# lemon produces from it is passed explicitly since there's nothing under
# csrc/ for file(GLOB csrc/*.c) to find.
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppLibrary.cmake)
sqlite_cpp_add_library(compiler-parser ${SQLITE_GENDIR}/parse.c)
