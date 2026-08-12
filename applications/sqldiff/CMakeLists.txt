cmake_minimum_required(VERSION 3.24)
project(sqldiff LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteDependency.cmake)
sqlite_require_core()

add_executable(sqldiff ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/sqldiff.c)
target_include_directories(sqldiff PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/misc)
target_link_libraries(sqldiff PRIVATE sqlite3::sqlite3)

include(GNUInstallDirs)
install(TARGETS sqldiff RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
