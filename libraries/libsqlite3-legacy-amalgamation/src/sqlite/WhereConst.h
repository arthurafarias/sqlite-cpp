
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct WhereConst WhereConst;
  struct WhereConst {
    Parse *pParse;
    u8 *pOomFault;
    int nConst;
    int nChng;
    int bHasAffBlob;
    u32 mExcludeOn;

    Expr **apExpr;
  };

  void constInsert(WhereConst * pConst, Expr * pColumn, Expr * pValue, Expr * pExpr);
  void findConstInWhere(WhereConst * pConst, Expr * pExpr);
  int propagateConstantExprRewriteOne(WhereConst * pConst, Expr * pExpr, int bIgnoreAffBlob);

#ifdef __cplusplus
}
#endif
