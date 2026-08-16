
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/sqlite3_pcache_page.h"

  typedef struct PgHdr1 PgHdr1;
  struct PgHdr1 {
    sqlite3_pcache_page page;
    unsigned int iKey;
    u16 isBulkLocal;
    u16 isAnchor;
    PgHdr1 *pNext;
    PCache1 *pCache;
    PgHdr1 *pLruNext;
    PgHdr1 *pLruPrev;
  };

  void pcache1FreePage(PgHdr1 * p);
  PgHdr1 *pcache1PinPage(PgHdr1 * pPage);
  void pcache1RemoveFromHash(PgHdr1 * pPage, int freeFlag);

#ifdef __cplusplus
}
#endif
