cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-4: relocated from tool/mkopcodec.tcl into applications/,
# renamed mkopcodec-legacy. Unlike lemon-legacy/mkkeywordhash-legacy, this is
# a TCL script (not compiled C), invoked directly by tclsh -- there is no
# executable target to build. This CMakeLists.txt exists only so the
# relocated tool has its own "proper applications/ target" directory (per
# SRS 001 FR-20's convention, as RR-4 requires) with the script attached to
# an IDE-visible target; cmake/SqliteCodegen.cmake invokes
# applications/mkopcodec-legacy/mkopcodec.tcl directly by path to generate
# opcodes.c, exactly as it invoked tool/mkopcodec.tcl before this move. Its
# own complete C++ replacement (RR-5, applications/mkopcodec) is deferred
# follow-up work, sequenced alongside sqlite-core-virtual-machine (SRS 002
# S5.4.2), not part of this relocation.
project(mkopcodec-legacy NONE)

add_custom_target(mkopcodec-legacy SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/mkopcodec.tcl)
