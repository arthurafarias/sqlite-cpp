cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-2: renamed from `sqldiff`. Its unsuffixed name is reserved
# (RR-3) for the eventual sqlite-cpp-linked replacement.
project(sqldiff-legacy LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteDependency.cmake)
sqlite_require_core()

add_executable(sqldiff-legacy ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/sqldiff.c)
target_include_directories(sqldiff-legacy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/misc)
target_link_libraries(sqldiff-legacy PRIVATE libsqlite3-legacy::libsqlite3-legacy)

include(GNUInstallDirs)
install(TARGETS sqldiff-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
