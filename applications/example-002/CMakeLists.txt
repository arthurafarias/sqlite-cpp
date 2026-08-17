cmake_minimum_required(VERSION 3.16)
project(sqlite3-cpp-applications-example-002)

find_package(SQLite3 REQUIRED)

add_executable(sqlite-cpp-applications-example-002 src/main.c)
target_link_libraries(sqlite-cpp-applications-example-002 PUBLIC sqlite3)