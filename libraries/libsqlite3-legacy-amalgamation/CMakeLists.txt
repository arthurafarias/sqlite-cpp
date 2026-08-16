cmake_minimum_required(VERSION 3.24)
project(sqlite3-legacy-amalgamation VERSION 3.53.4)

find_package(Threads REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_C_SOURCES src/*.c)
file(GLOB_RECURSE ${PROJECT_NAME}_CXX_SOURCES src/*.cpp)
set(${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_C_SOURCES} ${${PROJECT_NAME}_CXX_SOURCES})

add_executable(${PROJECT_NAME}-test-exec ${${PROJECT_NAME}_SOURCES})
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME}-test-exec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
# The container sources were extracted from a preprocessed dump generated
# under C23 semantics (glibc's stdarg.h expands va_start to the
# __builtin_c23_va_start form); an older C standard leaves that builtin
# unrecognized.
target_compile_features(${PROJECT_NAME}-test-exec PRIVATE c_std_23)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_23)
find_library(SQLITE3_LEGACY_AMALGAMATION_MATH_LIB m)
target_link_libraries(${PROJECT_NAME}-test-exec PRIVATE Threads::Threads ${SQLITE3_LEGACY_AMALGAMATION_MATH_LIB})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${SQLITE3_LEGACY_AMALGAMATION_MATH_LIB})