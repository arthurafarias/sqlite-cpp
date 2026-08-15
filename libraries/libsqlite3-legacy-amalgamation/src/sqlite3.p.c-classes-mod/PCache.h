
#pragma once
#include "PgHdr.h"
#ifdef __cplusplus
extern C {
#endif

#include "Pgno.h"

  typedef struct PCache PCache;

  typedef struct sqlite3_pcache_page sqlite3_pcache_page;
  typedef struct PgHdr PgHdr;

  static sqlite3_pcache_page *sqlite3PcacheFetch(PCache *, Pgno, int createFlag);
  static int sqlite3PcacheFetchStress(PCache *, Pgno, sqlite3_pcache_page **);
  static PgHdr *sqlite3PcacheFetchFinish(PCache *, Pgno, sqlite3_pcache_page * pPage);
  static void sqlite3PcacheCleanAll(PCache *);
  static void sqlite3PcacheClearWritable(PCache *);
  static void sqlite3PcacheTruncate(PCache *, Pgno x);
  static PgHdr *sqlite3PcacheDirtyList(PCache *);
  static void sqlite3PcacheClose(PCache *);
  static void sqlite3PcacheClearSyncFlags(PCache *);
  static void sqlite3PcacheClear(PCache *);
  static i64 sqlite3PcacheRefCount(PCache *);
  static int sqlite3PcachePagecount(PCache *);
  static void sqlite3PcacheSetCachesize(PCache *, int);
  static int sqlite3PcacheSetSpillsize(PCache *, int);
  static void sqlite3PcacheShrink(PCache *);
  static int sqlite3PCachePercentDirty(PCache *);
  static int sqlite3PCacheIsDirty(PCache * pCache);

#ifdef __cplusplus
}
#endif
