
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct RowSet RowSet;

struct RowSet {
  struct RowSetChunk *pChunk;
  sqlite3 *db;
  struct RowSetEntry *pEntry;
  struct RowSetEntry *pLast;
  struct RowSetEntry *pFresh;
  struct RowSetEntry *pForest;
  u16 nFresh;
  u16 rsFlags;
  int iBatch;
};

#ifdef __cplusplus
}
#endif

