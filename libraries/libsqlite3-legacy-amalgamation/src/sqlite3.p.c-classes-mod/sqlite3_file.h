#ifdef __cplusplus
extern "C" {
#endif

#include "i64.h"

typedef struct sqlite3_file sqlite3_file;
typedef struct sqlite3_io_methods sqlite3_io_methods;

struct sqlite3_file {
  const struct sqlite3_io_methods *pMethods;
};

static void sqlite3OsClose(sqlite3_file *);
static int sqlite3OsRead(sqlite3_file *, void *, int amt, i64 offset);
static int sqlite3OsWrite(sqlite3_file *, const void *, int amt, i64 offset);
static int sqlite3OsTruncate(sqlite3_file *, i64 size);
static int sqlite3OsSync(sqlite3_file *, int);
static int sqlite3OsFileSize(sqlite3_file *, i64 *pSize);
static int sqlite3OsLock(sqlite3_file *, int);
static int sqlite3OsUnlock(sqlite3_file *, int);
static int sqlite3OsCheckReservedLock(sqlite3_file *id, int *pResOut);
static int sqlite3OsFileControl(sqlite3_file *, int, void *);
static void sqlite3OsFileControlHint(sqlite3_file *, int, void *);
static int sqlite3OsSectorSize(sqlite3_file *id);
static int sqlite3OsDeviceCharacteristics(sqlite3_file *id);
static int sqlite3OsShmMap(sqlite3_file *, int, int, int, void volatile **);
static int sqlite3OsShmLock(sqlite3_file *id, int, int, int);
static void sqlite3OsShmBarrier(sqlite3_file *id);
static int sqlite3OsShmUnmap(sqlite3_file *id, int);
static int sqlite3OsFetch(sqlite3_file *id, i64, int, void **);
static int sqlite3OsUnfetch(sqlite3_file *, i64, void *);

#ifdef __cplusplus
}
#endif