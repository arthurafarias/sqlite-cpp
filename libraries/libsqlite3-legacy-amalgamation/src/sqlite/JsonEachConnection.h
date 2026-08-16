
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/sqlite3_vtab.h"

  typedef struct JsonEachConnection JsonEachConnection;
  struct JsonEachConnection {
    sqlite3_vtab base;
    sqlite3 *db;
    u8 eMode;
    u8 bRecursive;
  };

#ifdef __cplusplus
}
#endif
