
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/sqlite3_vtab_cursor.h"
#include "sqlite/JsonString.h"
#include "sqlite/JsonParse.h"

typedef struct JsonEachCursor JsonEachCursor;
struct JsonEachCursor {
  sqlite3_vtab_cursor base;
  u32 iRowid;
  u32 i;
  u32 iEnd;
  u32 nRoot;
  u8 eType;
  u8 bRecursive;
  u8 eMode;
  u32 nParent;
  u32 nParentAlloc;
  JsonParent *aParent;
  sqlite3 *db;
  JsonString path;
  JsonParse sParse;
};

#ifdef __cplusplus
}
#endif

