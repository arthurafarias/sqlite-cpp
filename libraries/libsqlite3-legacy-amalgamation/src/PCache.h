
#pragma once
#include "PgHdr.h"
#ifdef __cplusplus
extern C {
#endif

#include "Pgno.h"

  typedef struct PCache PCache;

  typedef struct sqlite3_pcache_page sqlite3_pcache_page;
  typedef struct PgHdr PgHdr;

  sqlite3_pcache_page *sqlite3PcacheFetch(PCache *, Pgno, int createFlag);
  int sqlite3PcacheFetchStress(PCache *, Pgno, sqlite3_pcache_page **);
  PgHdr *sqlite3PcacheFetchFinish(PCache *, Pgno, sqlite3_pcache_page * pPage);
  void sqlite3PcacheCleanAll(PCache *);
  void sqlite3PcacheClearWritable(PCache *);
  void sqlite3PcacheTruncate(PCache *, Pgno x);
  PgHdr *sqlite3PcacheDirtyList(PCache *);
  void sqlite3PcacheClose(PCache *);
  void sqlite3PcacheClearSyncFlags(PCache *);
  void sqlite3PcacheClear(PCache *);
  i64 sqlite3PcacheRefCount(PCache *);
  int sqlite3PcachePagecount(PCache *);
  void sqlite3PcacheSetCachesize(PCache *, int);
  int sqlite3PcacheSetSpillsize(PCache *, int);
  void sqlite3PcacheShrink(PCache *);
  int sqlite3PCachePercentDirty(PCache *);
  int sqlite3PCacheIsDirty(PCache * pCache);
  int sqlite3PcacheSetPageSize(PCache *, int);
  
#ifdef __cplusplus
}
#endif
