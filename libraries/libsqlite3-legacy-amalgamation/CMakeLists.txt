cmake_minimum_required(VERSION 3.24)
project(sqlite3-legacy-amalgamation VERSION 3.53.4)

find_package(Threads REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_C_SOURCES src/*.c)
file(GLOB_RECURSE ${PROJECT_NAME}_CXX_SOURCES src/*.cpp)
set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_C_SOURCES} ${${PROJECT_NAME}_CXX_SOURCES})

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)