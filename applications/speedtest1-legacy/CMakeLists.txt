cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-2/component map: relocated from benchmarks/ (a pre-existing gap
# vs. SRS 001 FR-20, which already expected speedtest1 to be its own
# applications/ target) and renamed from `speedtest1`. Unlike the other
# sqlite3-dependent applications, speedtest1-legacy is not a stepping stone
# to be superseded: both it and the eventual `speedtest1` are meant to
# coexist permanently as SRS 001 acceptance criterion 4's performance
# comparison fixture (SRS 002 S5.4.1).
project(speedtest1-legacy LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)

add_executable(speedtest1-legacy ${CMAKE_CURRENT_SOURCE_DIR}/../../test/speedtest1.c ${SQLITE_AMALGAMATION_C})
add_dependencies(speedtest1-legacy sqlite3_amalgamation)
target_include_directories(speedtest1-legacy PRIVATE ${SQLITE_GENDIR})
target_compile_definitions(speedtest1-legacy PRIVATE ${SQLITE_FEATURE_DEFS} SQLITE_OS_KV_OPTIONAL)
target_link_libraries(speedtest1-legacy PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
find_package(Threads REQUIRED)
target_link_libraries(speedtest1-legacy PRIVATE Threads::Threads)

include(GNUInstallDirs)
install(TARGETS speedtest1-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
