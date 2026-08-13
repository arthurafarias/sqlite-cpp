cmake_minimum_required(VERSION 3.24)
# A REPL demonstrating sqlite-core-command-processor's single-table SELECT
# executor end-to-end against a hardcoded in-memory demo table. Deliberately
# not named sqlite3-shell -- see main.cpp's header comment and SRS 002 RR-3.
project(sqlite-cpp-repl LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_command_processor()

add_executable(sqlite-cpp-repl main.cpp)
target_link_libraries(sqlite-cpp-repl PRIVATE sqlite::core::command_processor)
target_compile_features(sqlite-cpp-repl PRIVATE cxx_std_20)

include(GNUInstallDirs)
install(TARGETS sqlite-cpp-repl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
