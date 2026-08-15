
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Select Select;



struct Select {
  u8 op;
  LogEst nSelectRow;
  u32 selFlags;
  int iLimit, iOffset;
  u32 selId;
  ExprList *pEList;
  SrcList *pSrc;
  Expr *pWhere;
  ExprList *pGroupBy;
  Expr *pHaving;
  ExprList *pOrderBy;
  Select *pPrior;
  Select *pNext;
  Expr *pLimit;
  With *pWith;

  Window *pWin;
  Window *pWinDefn;
};

#ifdef __cplusplus
}
#endif

