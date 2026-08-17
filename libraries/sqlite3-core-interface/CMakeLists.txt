cmake_minimum_required(VERSION 3.24)
project(sqlite3-core-interface LANGUAGES C)

# SRS 005: see cmake/SqliteCppLibrary.cmake for what this does and why no
# sibling sqlite-cpp library is target_link_libraries()'d here.
sqlite3_cpp_add_split_library(sqlite3-core-interface)
