#include "sqlite/_All.h"

int numberOfCachePages(PCache *p) {
  if (p->szCache >= 0) {

    return p->szCache;
  } else {
    i64 n;

    n = ((-1024 * (i64)p->szCache) / (p->szPage + p->szExtra));
    if (n > 1000000000)
      n = 1000000000;
    return (int)n;
  }
}

int sqlite3PcacheSetPageSize(PCache *pCache, int szPage) {

  if (pCache->szPage) {
    sqlite3_pcache *pNew;
    pNew = sqlite3Config.pcache2.xCreate(szPage, pCache->szExtra + (((sizeof(PgHdr)) + 7) & ~7), pCache->bPurgeable);
    if (pNew == 0)
      return 7;
    sqlite3Config.pcache2.xCachesize(pNew, numberOfCachePages(pCache));
    if (pCache->pCache) {
      sqlite3Config.pcache2.xDestroy(pCache->pCache);
    }
    pCache->pCache = pNew;
    pCache->szPage = szPage;
    ;
  }
  return 0;
}

sqlite3_pcache_page *sqlite3PcacheFetch(PCache *pCache, Pgno pgno, int createFlag) {
  int eCreate;
  sqlite3_pcache_page *pRes;

  eCreate = createFlag & pCache->eCreate;

  pRes = sqlite3Config.pcache2.xFetch(pCache->pCache, pgno, eCreate);

  ;
  ;
  return pRes;
}

int sqlite3PcacheFetchStress(PCache *pCache, Pgno pgno, sqlite3_pcache_page **ppPage) {
  PgHdr *pPg;
  if (pCache->eCreate == 2)
    return 0;

  if (sqlite3PcachePagecount(pCache) > pCache->szSpill) {

    for (pPg = pCache->pSynced; pPg && (pPg->nRef || (pPg->flags & 0x008)); pPg = pPg->pDirtyPrev)
      ;
    pCache->pSynced = pPg;
    if (!pPg) {
      for (pPg = pCache->pDirtyTail; pPg && pPg->nRef; pPg = pPg->pDirtyPrev)
        ;
    }
    if (pPg) {
      int rc;

      ;
      rc = pCache->xStress(pCache->pStress, pPg);
      ;
      if (rc != 0 && rc != 5) {
        return rc;
      }
    }
  }
  *ppPage = sqlite3Config.pcache2.xFetch(pCache->pCache, pgno, 2);
  return *ppPage == 0 ? 7 : 0;
}

__attribute__((noinline)) PgHdr *pcacheFetchFinishWithInit(PCache *pCache, Pgno pgno, sqlite3_pcache_page *pPage) {
  PgHdr *pPgHdr;

  pPgHdr = (PgHdr *)pPage->pExtra;

  memset(&pPgHdr->pDirty, 0,
         sizeof(PgHdr) -

             __builtin_offsetof(

                 PgHdr

                 ,

                 pDirty

                 )

  );
  pPgHdr->pPage = pPage;
  pPgHdr->pData = pPage->pBuf;
  pPgHdr->pExtra = (void *)&pPgHdr[1];
  memset(pPgHdr->pExtra, 0, 8);

  pPgHdr->pCache = pCache;
  pPgHdr->pgno = pgno;
  pPgHdr->flags = 0x001;
  return sqlite3PcacheFetchFinish(pCache, pgno, pPage);
}

PgHdr *sqlite3PcacheFetchFinish(PCache *pCache, Pgno pgno, sqlite3_pcache_page *pPage) {
  PgHdr *pPgHdr;

  pPgHdr = (PgHdr *)pPage->pExtra;

  if (!pPgHdr->pPage) {
    return pcacheFetchFinishWithInit(pCache, pgno, pPage);
  }
  pCache->nRefSum++;
  pPgHdr->nRef++;

  return pPgHdr;
}

void sqlite3PcacheCleanAll(PCache *pCache) {
  PgHdr *p;
  ;
  while ((p = pCache->pDirty) != 0) {
    sqlite3PcacheMakeClean(p);
  }
}

void sqlite3PcacheClearWritable(PCache *pCache) {
  PgHdr *p;
  ;
  for (p = pCache->pDirty; p; p = p->pDirtyNext) {
    p->flags &= ~(0x008 | 0x004);
  }
  pCache->pSynced = pCache->pDirtyTail;
}

void sqlite3PcacheClearSyncFlags(PCache *pCache) {
  PgHdr *p;
  for (p = pCache->pDirty; p; p = p->pDirtyNext) {
    p->flags &= ~0x008;
  }
  pCache->pSynced = pCache->pDirtyTail;
}

void sqlite3PcacheTruncate(PCache *pCache, Pgno pgno) {
  if (pCache->pCache) {
    PgHdr *p;
    PgHdr *pNext;
    ;
    for (p = pCache->pDirty; p; p = pNext) {
      pNext = p->pDirtyNext;

      ((void)(0))

          ;
      if (p->pgno > pgno) {

        ((void)(0))

            ;
        sqlite3PcacheMakeClean(p);
      }
    }
    if (pgno == 0 && pCache->nRefSum) {
      sqlite3_pcache_page *pPage1;
      pPage1 = sqlite3Config.pcache2.xFetch(pCache->pCache, 1, 0);
      if ((pPage1)) {

        memset(pPage1->pBuf, 0, pCache->szPage);
        pgno = 1;
      }
    }
    sqlite3Config.pcache2.xTruncate(pCache->pCache, pgno + 1);
  }
}

void sqlite3PcacheClose(PCache *pCache) {

  ;
  sqlite3Config.pcache2.xDestroy(pCache->pCache);
}

void sqlite3PcacheClear(PCache *pCache) { sqlite3PcacheTruncate(pCache, 0); }

PgHdr *sqlite3PcacheDirtyList(PCache *pCache) {
  PgHdr *p;
  for (p = pCache->pDirty; p; p = p->pDirtyNext) {
    p->pDirty = p->pDirtyNext;
  }
  return pcacheSortDirtyList(pCache->pDirty);
}

i64 sqlite3PcacheRefCount(PCache *pCache) { return pCache->nRefSum; }

int sqlite3PcachePagecount(PCache *pCache) { return sqlite3Config.pcache2.xPagecount(pCache->pCache); }

void sqlite3PcacheSetCachesize(PCache *pCache, int mxPage) {

  pCache->szCache = mxPage;
  sqlite3Config.pcache2.xCachesize(pCache->pCache, numberOfCachePages(pCache));
}

int sqlite3PcacheSetSpillsize(PCache *p, int mxPage) {
  int res;

  if (mxPage) {
    if (mxPage < 0) {
      mxPage = (int)((-1024 * (i64)mxPage) / (p->szPage + p->szExtra));
    }
    p->szSpill = mxPage;
  }
  res = numberOfCachePages(p);
  if (res < p->szSpill)
    res = p->szSpill;
  return res;
}

void sqlite3PcacheShrink(PCache *pCache) { sqlite3Config.pcache2.xShrink(pCache->pCache); }

int sqlite3PCachePercentDirty(PCache *pCache) {
  PgHdr *pDirty;
  int nDirty = 0;
  int nCache = numberOfCachePages(pCache);
  for (pDirty = pCache->pDirty; pDirty; pDirty = pDirty->pDirtyNext)
    nDirty++;
  return nCache ? (int)(((i64)nDirty * 100) / nCache) : 0;
}

int sqlite3PCacheIsDirty(PCache *pCache) { return (pCache->pDirty != 0); }

void sqlite3PCacheBufferSetup(void *pBuf, int sz, int n) {
  if ((pcache1_g).isInit) {
    PgFreeslot *p;
    if (pBuf == 0)
      sz = n = 0;
    if (n == 0)
      sz = 0;
    sz = ((sz) & ~7);
    (pcache1_g).szSlot = sz;
    (pcache1_g).nSlot = (pcache1_g).nFreeSlot = n;
    (pcache1_g).nReserve = n > 90 ? 10 : (n / 10 + 1);
    (pcache1_g).pStart = pBuf;
    (pcache1_g).pFree = 0;
    __atomic_store_n((&(pcache1_g).bUnderPressure), (0), 0);
    while (n--) {
      p = (PgFreeslot *)pBuf;
      p->pNext = (pcache1_g).pFree;
      (pcache1_g).pFree = p;
      pBuf = (void *)&((char *)pBuf)[sz];
    }
    (pcache1_g).pEnd = pBuf;
  }
}

void sqlite3PCacheSetDefault(void) {
  static const sqlite3_pcache_methods2 defaultMethods = {1, 0, pcache1Init, pcache1Shutdown, pcache1Create, pcache1Cachesize, pcache1Pagecount, pcache1Fetch, pcache1Unpin, pcache1Rekey, pcache1Truncate, pcache1Destroy, pcache1Shrink};
  sqlite3_config(18, &defaultMethods);
}
