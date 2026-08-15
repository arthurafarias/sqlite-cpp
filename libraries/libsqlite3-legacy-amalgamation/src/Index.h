
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct Index Index;


struct Index {
  char *zName;
  i16 *aiColumn;
  LogEst *aiRowLogEst;
  Table *pTable;
  char *zColAff;
  Index *pNext;
  Schema *pSchema;
  u8 *aSortOrder;
  const char **azColl;
  Expr *pPartIdxWhere;
  ExprList *aColExpr;
  Pgno tnum;
  LogEst szIdxRow;
  u16 nKeyCol;
  u16 nColumn;
  u8 onError;
  unsigned idxType : 2;
  unsigned bUnordered : 1;
  unsigned uniqNotNull : 1;
  unsigned isResized : 1;
  unsigned isCovering : 1;
  unsigned noSkipScan : 1;
  unsigned hasStat1 : 1;
  unsigned bNoQuery : 1;
  unsigned bAscKeyBug : 1;
  unsigned bHasVCol : 1;
  unsigned bHasExpr : 1;
  Bitmask colNotIdxed;
};

#ifdef __cplusplus
}
#endif

