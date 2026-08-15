#pragma once

#ifdef __cplusplus

#extern "C" {
#endif

#include "sqlite/Btree.h"
#include "sqlite/Pager.h"
#include "sqlite/sqlite3_backup.h"
#include "sqlite/sqlite3_filename.h"
#include "sqlite/sqlite3_int64.h"
#include "sqlite/sqlite3_syscall_ptr.h"

typedef struct sqlite3_vfs sqlite3_vfs;
typedef struct sqlite3_file sqlite3_file;
typedef struct sqlite3 sqlite3;
typedef struct PgHdr DbPage;

struct sqlite3_vfs {
  int iVersion;
  int szOsFile;
  int mxPathname;
  sqlite3_vfs *pNext;
  const char *zName;
  void *pAppData;

  int (*xOpen)(sqlite3_vfs *, sqlite3_filename zName, sqlite3_file *, int flags, int *pOutFlags);
  int (*xDelete)(sqlite3_vfs *, const char *zName, int syncDir);
  int (*xAccess)(sqlite3_vfs *, const char *zName, int flags, int *pResOut);
  int (*xFullPathname)(sqlite3_vfs *, const char *zName, int nOut, char *zOut);
  void *(*xDlOpen)(sqlite3_vfs *, const char *zFilename);
  void (*xDlError)(sqlite3_vfs *, int nByte, char *zErrMsg);
  void (*(*xDlSym)(sqlite3_vfs *, void *, const char *zSymbol))(void);
  void (*xDlClose)(sqlite3_vfs *, void *);
  int (*xRandomness)(sqlite3_vfs *, int nByte, char *zOut);
  int (*xSleep)(sqlite3_vfs *, int microseconds);
  int (*xCurrentTime)(sqlite3_vfs *, double *);
  int (*xGetLastError)(sqlite3_vfs *, int, char *);

  int (*xCurrentTimeInt64)(sqlite3_vfs *, sqlite3_int64 *);

  int (*xSetSystemCall)(sqlite3_vfs *, const char *zName, sqlite3_syscall_ptr);
  sqlite3_syscall_ptr (*xGetSystemCall)(sqlite3_vfs *, const char *zName);
  const char *(*xNextSystemCall)(sqlite3_vfs *, const char *zName);
};

sqlite3_vfs *sqlite3_vfs_find(const char *zVfsName);
int sqlite3_vfs_register(sqlite3_vfs *, int makeDflt);
int sqlite3_vfs_unregister(sqlite3_vfs *);

int sqlite3OsOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
int sqlite3OsDelete(sqlite3_vfs *, const char *, int);
int sqlite3OsAccess(sqlite3_vfs *, const char *, int, int *pResOut);
int sqlite3OsFullPathname(sqlite3_vfs *, const char *, int, char *);
void *sqlite3OsDlOpen(sqlite3_vfs *, const char *);
void sqlite3OsDlError(sqlite3_vfs *, int, char *);
void (*sqlite3OsDlSym(sqlite3_vfs *, void *, const char *))(void);
void sqlite3OsDlClose(sqlite3_vfs *, void *);
int sqlite3OsRandomness(sqlite3_vfs *, int, char *);
int sqlite3OsSleep(sqlite3_vfs *, int);
int sqlite3OsGetLastError(sqlite3_vfs *);
int sqlite3OsCurrentTimeInt64(sqlite3_vfs *, sqlite3_int64 *);
int sqlite3OsOpenMalloc(sqlite3_vfs *, const char *, sqlite3_file **, int, int *);
int sqlite3PagerOpen(sqlite3_vfs *, Pager **ppPager, const char *, int, int, int, void (*)(DbPage *));
int sqlite3BtreeOpen(sqlite3_vfs *pVfs, const char *zFilename, sqlite3 *db, Btree **ppBtree, int flags, int vfsFlags);
int sqlite3JournalOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int);
int sqlite3JournalSize(sqlite3_vfs *);

#ifdef __cplusplus
}
#endif
