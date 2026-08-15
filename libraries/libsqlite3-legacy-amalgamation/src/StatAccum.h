
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct StatAccum StatAccum;

typedef struct StatAccum StatAccum;
struct StatAccum {
  sqlite3 *db;
  tRowcnt nEst;
  tRowcnt nRow;
  int nLimit;
  int nCol;
  int nKeyCol;
  u8 nSkipAhead;
  StatSample current;
};

#ifdef __cplusplus
}
#endif

