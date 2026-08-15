
#pragma once

#ifdef __cplusplus
extern C {
#endif

#include "Pgno.h"
#include "sqlite3_int64.h"
#include "DbPage.h"
#include "u8.h"
#include "u64.h"

typedef struct sqlite3_backup sqlite3_backup;
typedef struct sqlite3 sqlite3;
typedef struct Pager Pager;
typedef struct sqlite3_vfs sqlite3_vfs;
typedef struct sqlite3_file sqlite3_file;

static int sqlite3PagerClose(Pager *pPager, sqlite3 *);
static int sqlite3PagerReadFileheader(Pager *, int, unsigned char *);
static void sqlite3PagerSetBusyHandler(Pager *, int (*)(void *), void *);
static int sqlite3PagerSetPagesize(Pager *, u32 *, int);
static Pgno sqlite3PagerMaxPageCount(Pager *, Pgno);
static void sqlite3PagerSetCachesize(Pager *, int);
static int sqlite3PagerSetSpillsize(Pager *, int);
static void sqlite3PagerSetMmapLimit(Pager *, sqlite3_int64);
static void sqlite3PagerShrink(Pager *);
static void sqlite3PagerSetFlags(Pager *, unsigned);
static int sqlite3PagerLockingMode(Pager *, int);
static int sqlite3PagerSetJournalMode(Pager *, int);
static int sqlite3PagerGetJournalMode(Pager *);
static int sqlite3PagerOkToChangeJournalMode(Pager *);
static i64 sqlite3PagerJournalSizeLimit(Pager *, i64);
static sqlite3_backup **sqlite3PagerBackupPtr(Pager *);
static int sqlite3PagerFlush(Pager *);
static int sqlite3PagerGet(Pager *pPager, Pgno pgno, DbPage **ppPage, int clrFlag);
static DbPage *sqlite3PagerLookup(Pager *pPager, Pgno pgno);
static int sqlite3PagerMovepage(Pager *, DbPage *, Pgno, int);
static void sqlite3PagerPagecount(Pager *, int *);
static int sqlite3PagerBegin(Pager *, int exFlag, int);
static int sqlite3PagerCommitPhaseOne(Pager *, const char *zSuper, int);
static int sqlite3PagerExclusiveLock(Pager *);
static int sqlite3PagerSync(Pager *pPager, const char *zSuper);
static int sqlite3PagerCommitPhaseTwo(Pager *);
static int sqlite3PagerRollback(Pager *);
static int sqlite3PagerOpenSavepoint(Pager *pPager, int n);
static int sqlite3PagerSavepoint(Pager *pPager, int op, int iSavepoint);
static int sqlite3PagerSharedLock(Pager *pPager);
static int sqlite3PagerCheckpoint(Pager *pPager, sqlite3 *, int, int *, int *);
static int sqlite3PagerWalSupported(Pager *pPager);
static int sqlite3PagerWalCallback(Pager *pPager);
static int sqlite3PagerOpenWal(Pager *pPager, int *pisOpen);
static int sqlite3PagerCloseWal(Pager *pPager, sqlite3 *);
static int sqlite3PagerDirectReadOk(Pager *pPager, Pgno pgno);
static u8 sqlite3PagerIsreadonly(Pager *);
static u32 sqlite3PagerDataVersion(Pager *);
static int sqlite3PagerMemUsed(Pager *);
static sqlite3_vfs *sqlite3PagerVfs(Pager *);
static sqlite3_file *sqlite3PagerFile(Pager *);
static sqlite3_file *sqlite3PagerJrnlFile(Pager *);
static const char *sqlite3PagerJournalname(Pager *);
static void *sqlite3PagerTempSpace(Pager *);
static int sqlite3PagerIsMemdb(Pager *);
static void sqlite3PagerCacheStat(Pager *, int, int, u64 *);
static void sqlite3PagerClearCache(Pager *);

#ifdef __cplusplus
}
#endif

