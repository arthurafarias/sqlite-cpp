
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct JsonCache JsonCache;
  struct JsonCache {
    sqlite3 *db;
    int nUsed;
    JsonParse *a[4];
  };

  void jsonCacheDelete(JsonCache * p);
  void jsonCacheDeleteGeneric(void *p);

#ifdef __cplusplus
}
#endif
