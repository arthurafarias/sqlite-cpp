
#pragma once
#include "sqlite/i16.h"
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/ExprList.h"
#include "sqlite/u32.h"
#include "sqlite/u8.h"
#include "sqlite/With.h"

typedef i16 LogEst;
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
