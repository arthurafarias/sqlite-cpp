
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

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

