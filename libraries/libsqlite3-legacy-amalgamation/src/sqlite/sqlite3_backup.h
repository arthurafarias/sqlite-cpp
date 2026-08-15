
#pragma once
#include "sqlite/Btree.h"

#ifdef __cplusplus
extern C {
#endif

#include "sqlite/Pgno.h"
#include "sqlite/u32.h"

  typedef struct sqlite3 sqlite3;
  typedef struct Btree Btree;

  typedef struct sqlite3_backup sqlite3_backup;

  struct sqlite3_backup {
    sqlite3 *pDestDb;
    char *zDestDb;
    Btree *pDest;
    u32 iDestSchema;
    int bDestLocked;

    Pgno iNext;
    sqlite3 *pSrcDb;
    Btree *pSrc;

    int rc;

    Pgno nRemaining;
    Pgno nPagecount;

    int isAttached;
    sqlite3_backup *pNext;
  };

  int sqlite3_backup_step(sqlite3_backup * p, int nPage);
  int sqlite3_backup_finish(sqlite3_backup * p);
  int sqlite3_backup_remaining(sqlite3_backup * p);
  int sqlite3_backup_pagecount(sqlite3_backup * p);
  void sqlite3BackupRestart(sqlite3_backup *);
  void sqlite3BackupUpdate(sqlite3_backup *, Pgno, const u8 *);

#ifdef __cplusplus
}
#endif
