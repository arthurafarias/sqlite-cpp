
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct sqlite3_pcache_page sqlite3_pcache_page;

  struct sqlite3_pcache_page {
    void *pBuf;
    void *pExtra;
  };

#ifdef __cplusplus
}
#endif
