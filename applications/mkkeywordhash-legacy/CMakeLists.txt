cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-4: relocated from tool/mkkeywordhash.c into applications/,
# renamed mkkeywordhash-legacy. Standalone, dependency-free build: it has no
# sqlite3 dependency (SRS 002 S1.1) and no input file either -- the keyword
# list is compiled directly into the program (mirroring the original
# tool/mkkeywordhash.c). cmake/SqliteCodegen.cmake add_subdirectory()-falls-
# back to this directory to obtain the mkkeywordhash-legacy host tool it
# needs to generate keywordhash.h; its own complete C++ replacement (RR-5,
# applications/mkkeywordhash) is deferred follow-up work, not part of this
# relocation.
project(mkkeywordhash-legacy LANGUAGES C)

add_executable(mkkeywordhash-legacy ${CMAKE_CURRENT_SOURCE_DIR}/mkkeywordhash.c)
set_target_properties(mkkeywordhash-legacy PROPERTIES C_STANDARD 11)

include(GNUInstallDirs)
install(TARGETS mkkeywordhash-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
