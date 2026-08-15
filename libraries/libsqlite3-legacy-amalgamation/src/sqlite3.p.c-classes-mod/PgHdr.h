
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "i64.h"
#include "u16.h"

#include "Pgno.h"

  typedef struct PgHdr PgHdr;

  typedef struct sqlite3_pcache_page sqlite3_pcache_page;
  typedef struct PCache PCache;
  typedef struct Pager Pager;

  struct PgHdr {
    sqlite3_pcache_page *pPage;
    void *pData;
    void *pExtra;
    PCache *pCache;
    PgHdr *pDirty;
    Pager *pPager;

    Pgno pgno;
    u16 flags;

    i64 nRef;
    PgHdr *pDirtyNext;
    PgHdr *pDirtyPrev;
  };

#ifdef __cplusplus
}
#endif
