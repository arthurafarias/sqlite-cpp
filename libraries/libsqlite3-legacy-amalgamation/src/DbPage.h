#ifdef __cplusplus
extern "C" {
#endif

#include "Pgno.h"
#include "u16.h"

typedef struct PgHdr PgHdr;
typedef struct PgHdr DbPage;

void sqlite3PagerRekey(DbPage *, Pgno, u16);
void sqlite3PagerRef(DbPage *);
void sqlite3PagerUnref(DbPage *);
void sqlite3PagerUnrefNotNull(DbPage *);
void sqlite3PagerUnrefPageOne(DbPage *);
int sqlite3PagerWrite(DbPage *);
void sqlite3PagerDontWrite(DbPage *);
int sqlite3PagerPageRefcount(DbPage *);
void *sqlite3PagerGetData(DbPage *);
void *sqlite3PagerGetExtra(DbPage *);

#ifdef __cplusplus
}
#endif
