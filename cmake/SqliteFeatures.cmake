# Declares the SQLITE_ENABLE_* build options (mirroring autosetup/sqlite-config.tcl's
# allFlags) and computes the resulting SQLITE_FEATURE_DEFS / SQLITE_FEATURE_LIBS lists.
#
# In the real build these flags land in $(OPT_FEATURE_FLAGS), which folds into $(T.cc)
# globally - so they apply not only to libsqlite3-legacy but to the sqlite3 CLI shell's
# own build too (shell.c + sqlite3.c are compiled with $(T.link), which already carries
# $(OPT_FEATURE_FLAGS), plus $(SHELL_OPT) appended on top). Sharing this list between
# libraries/libsqlite3-legacy and applications/sqlite3-shell-legacy (each still adds its
# own shell-only/library-only extras) reproduces that, instead of the shell only getting
# its fixed OPT_SHELL set and silently missing e.g. math functions.
#
# Included once; defines, in the caller's scope: SQLITE_FEATURE_DEFS, SQLITE_FEATURE_LIBS.

if(SQLITE_FEATURES_INCLUDED)
    return()
endif()
set(SQLITE_FEATURES_INCLUDED TRUE CACHE INTERNAL "")

option(SQLITE_ENABLE_FTS3 "Enable the FTS3 full-text-search extension" OFF)
option(SQLITE_ENABLE_FTS4 "Enable the FTS4 full-text-search extension" OFF)
option(SQLITE_ENABLE_FTS5 "Enable the FTS5 full-text-search extension" OFF)
option(SQLITE_ENABLE_RTREE "Enable the R*Tree index extension" OFF)
option(SQLITE_ENABLE_GEOPOLY "Enable the Geopoly extension (implies SQLITE_ENABLE_RTREE)" OFF)
option(SQLITE_ENABLE_SESSION "Enable the session/changeset extension" OFF)
option(SQLITE_ENABLE_UPDATE_DELETE_LIMIT "Enable UPDATE/DELETE ... LIMIT syntax" OFF)
option(SQLITE_ENABLE_DBPAGE_VTAB "Enable the sqlite_dbpage virtual table" OFF)
option(SQLITE_ENABLE_DBSTAT_VTAB "Enable the dbstat virtual table" OFF)
option(SQLITE_ENABLE_MEMSYS5 "Use the memsys5 memory allocator" OFF)
option(SQLITE_ENABLE_MEMSYS3 "Use the memsys3 memory allocator (ignored if MEMSYS5 is on)" OFF)
# Empirically OFF in a plain `./configure && make` (verified against the real build's
# `pragma_compile_options()` output), despite the "carray=1" look of its allFlags entry.
option(SQLITE_ENABLE_CARRAY "Enable the carray() table-valued function" OFF)
option(SQLITE_ENABLE_MATH_FUNCTIONS "Enable built-in SQL math functions" ON)
option(SQLITE_ENABLE_JSON "Enable built-in JSON SQL functions" ON)
option(SQLITE_ENABLE_LOAD_EXTENSION "Enable sqlite3_load_extension()" ON)
option(SQLITE_ENABLE_ICU "Enable the ICU extension (requires ICU)" OFF)
option(SQLITE_THREADSAFE "Build a thread-safe library" ON)

set(SQLITE_FEATURE_DEFS BUILD_sqlite)
set(SQLITE_FEATURE_LIBS)
# Raw "-lfoo"-style flags mirroring SQLITE_FEATURE_LIBS, for the pkg-config .pc file:
# CMake imported-target names like ZLIB::ZLIB (used in SQLITE_FEATURE_LIBS/
# target_link_libraries) mean nothing to a plain-text .pc file read by non-CMake
# consumers, so that file needs this separate, pkg-config-safe list instead.
set(SQLITE_FEATURE_PC_LIBS)

if(SQLITE_ENABLE_FTS3)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_FTS3)
endif()
if(SQLITE_ENABLE_FTS4)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_FTS4)
endif()
if(SQLITE_ENABLE_FTS5)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_FTS5)
endif()
if(SQLITE_ENABLE_GEOPOLY)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_GEOPOLY SQLITE_ENABLE_RTREE)
elseif(SQLITE_ENABLE_RTREE)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_RTREE)
endif()
if(SQLITE_ENABLE_SESSION)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_SESSION SQLITE_ENABLE_PREUPDATE_HOOK)
endif()
if(SQLITE_ENABLE_UPDATE_DELETE_LIMIT)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_UPDATE_DELETE_LIMIT)
endif()
if(SQLITE_ENABLE_DBPAGE_VTAB)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_DBPAGE_VTAB)
endif()
if(SQLITE_ENABLE_DBSTAT_VTAB)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_DBSTAT_VTAB)
endif()
if(SQLITE_ENABLE_MEMSYS5)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_MEMSYS5)
elseif(SQLITE_ENABLE_MEMSYS3)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_MEMSYS3)
endif()
if(SQLITE_ENABLE_CARRAY)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_CARRAY)
endif()
if(SQLITE_ENABLE_MATH_FUNCTIONS)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_MATH_FUNCTIONS SQLITE_ENABLE_PERCENTILE)
endif()
if(NOT SQLITE_ENABLE_JSON)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_OMIT_JSON)
endif()
if(NOT SQLITE_ENABLE_LOAD_EXTENSION)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_OMIT_LOAD_EXTENSION=1)
endif()
if(SQLITE_THREADSAFE)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_THREADSAFE=1)
else()
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_THREADSAFE=0)
endif()

# zlib is auto-detected (not a --enable/--disable flag) in autosetup/sqlite-config.tcl,
# gating sqlar/zipfile compression support in ext/misc.
find_package(ZLIB QUIET)
if(ZLIB_FOUND)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_HAVE_ZLIB=1)
    list(APPEND SQLITE_FEATURE_LIBS ZLIB::ZLIB)
    list(APPEND SQLITE_FEATURE_PC_LIBS -lz)
endif()

# Upstream detects these at configure time into sqlite_cfg.h (included only under
# -D_HAVE_SQLITE_CONFIG_H, which this build does not define - the amalgamation
# builds fine without it, same as any hand-built "gcc -c sqlite3.c"). Passing the
# equivalent -DHAVE_X=1 flags directly reproduces the same os_unix.c/util.c code
# paths (pread/pwrite, fdatasync, posix_fallocate, ...) without needing sqlite_cfg.h.
include(CheckSymbolExists)
set(CMAKE_REQUIRED_LIBRARIES_SAVE ${CMAKE_REQUIRED_LIBRARIES})
set(CMAKE_REQUIRED_DEFINITIONS_SAVE ${CMAKE_REQUIRED_DEFINITIONS})
set(CMAKE_REQUIRED_LIBRARIES m)
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
# Encoded as "fn:header" (not "fn;header") because a CMake list is just a
# semicolon-joined string, so a semicolon here would silently flatten into two
# separate list elements instead of staying paired.
set(SQLITE_HAVE_CHECKS
    isnan:math.h
    gmtime_r:time.h
    localtime_r:time.h
    usleep:unistd.h
    nanosleep:time.h
    pread:unistd.h
    pwrite:unistd.h
    pread64:unistd.h
    pwrite64:unistd.h
    fdatasync:unistd.h
    posix_fallocate:fcntl.h
    lstat:sys/stat.h
    readlink:unistd.h
    fchmod:unistd.h
    fchown:unistd.h
    strchrnul:string.h
    strerror_r:string.h
    utime:utime.h
    dlopen:dlfcn.h
    malloc_usable_size:malloc.h
)
foreach(_check IN LISTS SQLITE_HAVE_CHECKS)
    string(REPLACE ":" ";" _check_pair "${_check}")
    list(GET _check_pair 0 _fn)
    list(GET _check_pair 1 _hdr)
    string(TOUPPER "${_fn}" _fn_upper)
    check_symbol_exists(${_fn} ${_hdr} SQLITE_HAVE_${_fn_upper})
    if(SQLITE_HAVE_${_fn_upper})
        list(APPEND SQLITE_FEATURE_DEFS HAVE_${_fn_upper}=1)
    endif()
endforeach()
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_SAVE})
set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS_SAVE})

if(SQLITE_ENABLE_ICU)
    find_package(PkgConfig REQUIRED)
    # Upstream's own autosetup only pkg-config's icu-io (its comment claims icu-io's
    # Requires pulls in icu-uc/icu-i18n "on tested OSes"); on this system icu-io alone
    # resolves to just -licuio, so all three are requested explicitly here to link.
    pkg_check_modules(ICU REQUIRED icu-uc icu-io icu-i18n)
    list(APPEND SQLITE_FEATURE_DEFS SQLITE_ENABLE_ICU)
    list(APPEND SQLITE_FEATURE_LIBS ${ICU_LIBRARIES})
    foreach(_icu_lib IN LISTS ICU_LIBRARIES)
        list(APPEND SQLITE_FEATURE_PC_LIBS -l${_icu_lib})
    endforeach()
endif()
