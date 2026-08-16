#include "sqlite/_All.h"

void jsonCacheDelete(JsonCache *p) {
  int i;
  for (i = 0; i < p->nUsed; i++) {
    jsonParseFree(p->a[i]);
  }
  sqlite3DbFree(p->db, p);
}

void jsonCacheDeleteGeneric(void *p) { jsonCacheDelete((JsonCache *)p); }
