
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct Incrblob Incrblob;
  struct Incrblob {
    int nByte;
    int iOffset;
    u16 iCol;
    BtCursor *pCsr;
    sqlite3_stmt *pStmt;
    sqlite3 *db;
    char *zDb;
    Table *pTab;
  };

  int blobSeekToRow(Incrblob * p, sqlite3_int64 iRow, char **pzErr);

#ifdef __cplusplus
}
#endif
