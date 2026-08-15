
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct PragmaVtabCursor PragmaVtabCursor;
struct PragmaVtabCursor {
  sqlite3_vtab_cursor base;
  sqlite3_stmt *pPragma;
  sqlite_int64 iRowid;
  char *azArg[2];
};

#ifdef __cplusplus
}
#endif

