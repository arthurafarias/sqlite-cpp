
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct Upsert Upsert;


struct Upsert {
  ExprList *pUpsertTarget;
  Expr *pUpsertTargetWhere;
  ExprList *pUpsertSet;
  Expr *pUpsertWhere;
  Upsert *pNextUpsert;
  u8 isDoUpdate;
  u8 isDup;

  void *pToFree;

  Index *pUpsertIdx;
  SrcList *pUpsertSrc;
  int regData;
  int iDataCur;
  int iIdxCur;
};

#ifdef __cplusplus
}
#endif

