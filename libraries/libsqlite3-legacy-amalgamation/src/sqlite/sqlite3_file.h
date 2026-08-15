#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/i64.h"

typedef struct sqlite3_file sqlite3_file;
typedef struct sqlite3_io_methods sqlite3_io_methods;

struct sqlite3_file {
  const struct sqlite3_io_methods *pMethods;
};

void sqlite3OsClose(sqlite3_file *);
int sqlite3OsRead(sqlite3_file *, void *, int amt, i64 offset);
int sqlite3OsWrite(sqlite3_file *, const void *, int amt, i64 offset);
int sqlite3OsTruncate(sqlite3_file *, i64 size);
int sqlite3OsSync(sqlite3_file *, int);
int sqlite3OsFileSize(sqlite3_file *, i64 *pSize);
int sqlite3OsLock(sqlite3_file *, int);
int sqlite3OsUnlock(sqlite3_file *, int);
int sqlite3OsCheckReservedLock(sqlite3_file *id, int *pResOut);
int sqlite3OsFileControl(sqlite3_file *, int, void *);
void sqlite3OsFileControlHint(sqlite3_file *, int, void *);
int sqlite3OsSectorSize(sqlite3_file *id);
int sqlite3OsDeviceCharacteristics(sqlite3_file *id);
int sqlite3OsShmMap(sqlite3_file *, int, int, int, void volatile **);
int sqlite3OsShmLock(sqlite3_file *id, int, int, int);
void sqlite3OsShmBarrier(sqlite3_file *id);
int sqlite3OsShmUnmap(sqlite3_file *id, int);
int sqlite3OsFetch(sqlite3_file *id, i64, int, void **);
int sqlite3OsUnfetch(sqlite3_file *, i64, void *);
void sqlite3OsCloseFree(sqlite3_file *);
int sqlite3SectorSize(sqlite3_file *);
int sqlite3JournalIsInMemory(sqlite3_file *p);
void sqlite3MemJournalOpen(sqlite3_file *);

#ifdef __cplusplus
}
#endif