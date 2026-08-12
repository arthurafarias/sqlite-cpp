# Cross-compilation toolchain file for building and testing the Windows body
# of sqlite-backend-os (windows_file.hpp/windows_vfs.hpp/win32_mutex.hpp)
# from a Linux host: this session's environment has no Windows machine
# available, so real Win32 code is instead cross-compiled with mingw-w64
# and its test binary run under Wine (see the top-level CMakeLists.txt's
# SQLITE_CPP_TEST_WINDOWS_OS option and libraries/libsqlite-backend-os's
# docs for how this fits FR-6's "two implementations selected by CMake
# if(WIN32)" requirement for a header-only library).
#
# Usage:
#   cmake -S libraries/libsqlite-backend-os -B build-windows \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-toolchain.cmake
#   cmake --build build-windows
#   wine build-windows/tests/sqlite_backend_os_tests.exe
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Statically link the mingw/libstdc++/pthread runtime DLLs so the resulting
# .exe runs under Wine without needing them separately installed/found.
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static-libgcc -static-libstdc++ -static")
