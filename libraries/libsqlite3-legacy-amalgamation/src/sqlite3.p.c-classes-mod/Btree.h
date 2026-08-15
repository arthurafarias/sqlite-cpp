
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "u32.h"
#include "u8.h"

#include "BtCursor.h"
#include "BtLock.h"
#include "KeyInfo.h"
#include "Pgno.h"
#include "sqlite3_int64.h"

  typedef struct Btree Btree;
  typedef struct sqlite3 sqlite3;
  typedef struct BtShared BtShared;
  typedef struct BtLock BtLock;
  typedef struct KeyInfo KeyInfo;

  struct Btree {
    sqlite3 *db;
    BtShared *pBt;
    u8 inTrans;
    u8 sharable;
    u8 locked;
    u8 hasIncrblobCur;
    int wantToLock;
    int nBackup;
    u32 iBDataVersion;
    Btree *pNext;
    Btree *pPrev;
    BtLock lock;
  };

  static int sqlite3BtreeClose(Btree *);
  static int sqlite3BtreeSetCacheSize(Btree *, int);
  static int sqlite3BtreeSetSpillSize(Btree *, int);
  static int sqlite3BtreeSetMmapLimit(Btree *, sqlite3_int64);
  static int sqlite3BtreeSetPagerFlags(Btree *, unsigned);
  static int sqlite3BtreeSetPageSize(Btree * p, int nPagesize, int nReserve, int eFix);
  static int sqlite3BtreeGetPageSize(Btree *);
  static Pgno sqlite3BtreeMaxPageCount(Btree *, Pgno);
  static Pgno sqlite3BtreeLastPage(Btree *);
  static int sqlite3BtreeSecureDelete(Btree *, int);
  static int sqlite3BtreeGetRequestedReserve(Btree *);
  static int sqlite3BtreeGetReserveNoMutex(Btree * p);
  static int sqlite3BtreeSetAutoVacuum(Btree *, int);
  static int sqlite3BtreeGetAutoVacuum(Btree *);
  static int sqlite3BtreeBeginTrans(Btree *, int, int *);
  static int sqlite3BtreeCommitPhaseOne(Btree *, const char *);
  static int sqlite3BtreeCommitPhaseTwo(Btree *, int);
  static int sqlite3BtreeCommit(Btree *);
  static int sqlite3BtreeRollback(Btree *, int, int);
  static int sqlite3BtreeBeginStmt(Btree *, int);
  static int sqlite3BtreeCreateTable(Btree *, Pgno *, int flags);
  static int sqlite3BtreeTxnState(Btree *);
  static int sqlite3BtreeIsInBackup(Btree *);
  static void *sqlite3BtreeSchema(Btree *, int, void (*)(void *));
  static int sqlite3BtreeSchemaLocked(Btree * pBtree);
  static int sqlite3BtreeLockTable(Btree * pBtree, int iTab, u8 isWriteLock);
  static int sqlite3BtreeSavepoint(Btree *, int, int);
  static int sqlite3BtreeCheckpoint(Btree *, int, int *, int *);
  static const char *sqlite3BtreeGetFilename(Btree *);
  static const char *sqlite3BtreeGetJournalname(Btree *);
  static int sqlite3BtreeCopyFile(Btree *, Btree *);
  static int sqlite3BtreeIncrVacuum(Btree *);
  static int sqlite3BtreeDropTable(Btree *, int, int *);
  static int sqlite3BtreeClearTable(Btree *, int, i64 *);
  static int sqlite3BtreeTripAllCursors(Btree *, int, int);
  static void sqlite3BtreeGetMeta(Btree * pBtree, int idx, u32 *pValue);
  static int sqlite3BtreeUpdateMeta(Btree *, int idx, u32 value);
  static int sqlite3BtreeNewDb(Btree * p);
  static int sqlite3BtreeCursor(Btree *, Pgno iTable, int wrFlag, struct KeyInfo *, BtCursor *pCursor);

#ifdef __cplusplus
}
#endif
