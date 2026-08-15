#include "PgHdr.h"

typedef struct PgHdr DbPage;

static void sqlite3PagerRekey(DbPage *, Pgno, u16);

static void sqlite3PagerRef(DbPage *);
static void sqlite3PagerUnref(DbPage *);
static void sqlite3PagerUnrefNotNull(DbPage *);
static void sqlite3PagerUnrefPageOne(DbPage *);
static int sqlite3PagerWrite(DbPage *);
static void sqlite3PagerDontWrite(DbPage *);
static int sqlite3PagerPageRefcount(DbPage *);
static void *sqlite3PagerGetData(DbPage *);
static void *sqlite3PagerGetExtra(DbPage *);