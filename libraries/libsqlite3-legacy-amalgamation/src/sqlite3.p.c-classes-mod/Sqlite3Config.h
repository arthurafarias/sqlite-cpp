
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct Sqlite3Config {
  int bMemstat;
  u8 bCoreMutex;
  u8 bFullMutex;
  u8 bOpenUri;
  u8 bUseCis;
  u8 bSmallMalloc;
  u8 bExtraSchemaChecks;

  int mxStrlen;
  int neverCorrupt;
  int szLookaside;
  int nLookaside;
  int nStmtSpill;
  sqlite3_mem_methods m;
  sqlite3_mutex_methods mutex;
  sqlite3_pcache_methods2 pcache2;
  void *pHeap;
  int nHeap;
  int mnReq, mxReq;
  sqlite3_int64 szMmap;
  sqlite3_int64 mxMmap;
  void *pPage;
  int szPage;
  int nPage;
  int mxParserStack;
  int sharedCacheEnabled;
  u32 szPma;

  int isInit;
  int inProgress;
  int isMutexInit;
  int isMallocInit;
  int isPCacheInit;
  int nRefInitMutex;
  sqlite3_mutex *pInitMutex;
  void (*xLog)(void *, int, const char *);
  void *pLogArg;
  sqlite3_int64 mxMemdbSize;

  int (*xTestCallback)(int);

  int bLocaltimeFault;
  int (*xAltLocaltime)(const void *, void *);
  int iOnceResetThreshold;
  u32 szSorterRef;
  unsigned int iPrngSeed;
};
static struct Sqlite3Config sqlite3Config = {
    1,
    1,
    1 == 1,
    0,
    1,
    0,
    1,

    0x7ffffffe,
    0,
    1200,
    40,
    (64 * 1024),
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    (void *)0,
    0,
    0,
    0,
    0,
    0x7fff0000,
    (void *)0,
    0,
    20,
    0,
    0,
    250,

    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1073741824,

    0,

    0,
    0,
    0x7ffffffe,
    0x7fffffff,
    0,

};

static struct Sqlite3Config sqlite3Config = {
    1,
    1,
    1 == 1,
    0,
    1,
    0,
    1,

    0x7ffffffe,
    0,
    1200,
    40,
    (64 * 1024),
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    (void *)0,
    0,
    0,
    0,
    0,
    0x7fff0000,
    (void *)0,
    0,
    20,
    0,
    0,
    250,

    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1073741824,

    0,

    0,
    0,
    0x7ffffffe,
    0x7fffffff,
    0,

};

#ifdef __cplusplus
}
#endif