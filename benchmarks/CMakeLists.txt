cmake_minimum_required(VERSION 3.24)
project(sqlite3-benchmarks LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteCodegen.cmake)

add_executable(speedtest1 ${CMAKE_CURRENT_SOURCE_DIR}/../test/speedtest1.c ${SQLITE_AMALGAMATION_C})
add_dependencies(speedtest1 sqlite3_amalgamation)
target_include_directories(speedtest1 PRIVATE ${SQLITE_GENDIR})
target_compile_definitions(speedtest1 PRIVATE ${SQLITE_FEATURE_DEFS} SQLITE_OS_KV_OPTIONAL)
target_link_libraries(speedtest1 PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
find_package(Threads REQUIRED)
target_link_libraries(speedtest1 PRIVATE Threads::Threads)
