cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-4: relocated from tool/lemon.c (with its lempar.c template,
# also tool/-only before this move) into applications/, renamed
# lemon-legacy. Standalone, dependency-free build: lemon is a
# self-contained LALR(1) parser generator with no sqlite3 dependency
# (SRS 002 S1.1), so unlike the sqlite3-dependent applications this has no
# sqlite_require_core()-style fallback to wire up. cmake/SqliteCodegen.cmake
# add_subdirectory()-falls-back to this directory to obtain the lemon-legacy
# host tool it needs to generate parse.c/h and fts5parse.c/h; its own
# complete C++ replacement (RR-5, applications/lemon) is deferred follow-up
# work, not part of this relocation.
project(lemon-legacy LANGUAGES C)

add_executable(lemon-legacy ${CMAKE_CURRENT_SOURCE_DIR}/lemon.c)
set_target_properties(lemon-legacy PROPERTIES C_STANDARD 11)

include(GNUInstallDirs)
install(TARGETS lemon-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lempar.c DESTINATION ${CMAKE_INSTALL_DATADIR}/lemon-legacy)
