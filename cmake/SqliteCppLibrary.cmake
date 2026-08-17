# SRS 005: shared helper for the ten SRS-001 sec.1.3 subsystem libraries once
# populated from sqlite3-legacy-alternate (see srs/005-*.md). Each library
# compiles independently -- no target_link_libraries() between siblings, since
# the real C symbol graph among them is cyclic and CMake refuses target-level
# cycles between SHARED libraries (SRS 001 FR-4's finding, unchanged by this
# document). What *is* needed across siblings is header visibility: a file
# physically placed in one library routinely #includes a container header
# that was categorized into another (e.g. Btree.c's own headers reference
# Pager.h). So every split library's public include/ directory is added as a
# compile-time (not link-time) search path to every other split library.
# Undefined symbols left in each .so by this are resolved later, once
# something links the whole set together -- that "something" is
# libraries/sqlite3, the umbrella target SRS 005 adds.

set(SQLITE_CPP_SPLIT_LIBRARIES
    sqlite3-utils
    sqlite3-backend-os
    sqlite3-backend-pager
    sqlite3-backend-tree
    sqlite3-core-virtual-machine
    sqlite3-core-command-processor
    sqlite3-core-interface
    sqlite3-compiler-tokenizer
    sqlite3-compiler-parser
    sqlite3-compiler-code-generator
)

function(sqlite3_cpp_add_split_library NAME)
    file(GLOB_RECURSE ${NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/csrc/*.c)
    add_library(${NAME} SHARED ${${NAME}_SOURCES})
    target_compile_features(${NAME} PRIVATE c_std_23)
    # Both the parent (for the tree's own "sqlite/X.h" include convention)
    # and the sqlite/ subdirectory itself are added: a handful of
    # sqlite3-legacy-alternate containers #include their own header as a
    # bare "X.h" rather than "sqlite/X.h" (e.g. bft.c -> "bft.h") -- that
    # only worked there because both files sat in the same flat directory,
    # which physically separating csrc/ from include/sqlite/ breaks unless
    # the sqlite/ directory is *also* on the search path directly.
    target_include_directories(${NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/sqlite>
    )
    foreach(sibling IN LISTS SQLITE_CPP_SPLIT_LIBRARIES)
        if(NOT sibling STREQUAL "${NAME}")
            target_include_directories(${NAME} PRIVATE
                ${CMAKE_SOURCE_DIR}/libraries/${sibling}/include
                ${CMAKE_SOURCE_DIR}/libraries/${sibling}/include/sqlite
            )
        endif()
    endforeach()

    find_package(Threads REQUIRED)
    find_library(SQLITE_CPP_MATH_LIB m)
    target_link_libraries(${NAME} PRIVATE Threads::Threads ${SQLITE_CPP_MATH_LIB})
endfunction()
