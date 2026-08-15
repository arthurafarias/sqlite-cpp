
#pragma once
#include "sqlite/Pgno.h"
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/StrAccum.h"

typedef struct IntegrityCk IntegrityCk;
struct IntegrityCk {
  BtShared *pBt;
  Pager *pPager;
  u8 *aPgRef;
  Pgno nCkPage;
  int mxErr;
  int nErr;
  int rc;
  u32 nStep;
  const char *zPfx;
  Pgno v0;
  Pgno v1;
  int v2;
  StrAccum errMsg;
  u32 *heap;
  sqlite3 *db;
  i64 nRow;
};

#ifdef __cplusplus
}
#endif

