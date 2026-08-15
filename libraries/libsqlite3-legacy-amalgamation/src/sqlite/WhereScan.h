
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct WhereScan WhereScan;
struct WhereScan {
  WhereClause *pOrigWC;
  WhereClause *pWC;
  const char *zCollName;
  Expr *pIdxExpr;
  int k;
  u32 opMask;
  char idxaff;
  unsigned char iEquiv;
  unsigned char nEquiv;
  int aiCur[11];
  i16 aiColumn[11];
};


#ifdef __cplusplus
}
#endif

