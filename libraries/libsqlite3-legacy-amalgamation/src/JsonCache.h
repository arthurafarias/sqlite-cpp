
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct JsonCache JsonCache;
struct JsonCache {
  sqlite3 *db;
  int nUsed;
  JsonParse *a[4];
};

#ifdef __cplusplus
}
#endif

