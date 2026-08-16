#include "sqlite/_All.h"

void analyzeAggFuncArgs(AggInfo *pAggInfo, NameContext *pNC) {
  int i;




  pNC->ncFlags |= 0x020000;
  for (i = 0; i < pAggInfo->nFunc; i++) {
    Expr *pExpr = pAggInfo->aFunc[i].pFExpr;

    ((void)(0))

        ;

    ((void)(0))

        ;
    sqlite3ExprAnalyzeAggList(pNC, pExpr->x.pList);
    if (pExpr->pLeft) {

      ((void)(0))

          ;

      ((void)(0))

          ;
      sqlite3ExprAnalyzeAggList(pNC, pExpr->pLeft->x.pList);
    }

    ((void)(0))

        ;
    if ((((pExpr)->flags & (u32)(0x1000000)) != 0)) {
      sqlite3ExprAnalyzeAggregates(pNC, pExpr->y.pWin->pFilter);
    }
  }
  pNC->ncFlags &= ~0x020000;
}

void aggregateConvertIndexedExprRefToColumn(AggInfo *pAggInfo) {
  int i;
  Walker w;
  memset(&w, 0, sizeof(w));
  w.xExprCallback = aggregateIdxEprRefToColCallback;
  for (i = 0; i < pAggInfo->nFunc; i++) {
    sqlite3WalkExpr(&w, pAggInfo->aFunc[i].pFExpr);
  }
}
