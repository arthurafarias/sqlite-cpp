cmake_minimum_required(VERSION 3.16)
project(sqlite-cpp-applications-example-001)

find_package(SQLite3 REQUIRED)

add_executable(sqlite-cpp-applications-example-001 src/main.c)
target_link_libraries(sqlite-cpp-applications-example-001 PUBLIC sqlite3-legacy-amalgamation)