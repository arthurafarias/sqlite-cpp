
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "Parse.h"
#include "u16.h"
#include "SrcList.h"

typedef struct Walker Walker;


struct Walker {
  Parse *pParse;
  int (*xExprCallback)(Walker *, Expr *);
  int (*xSelectCallback)(Walker *, Select *);
  void (*xSelectCallback2)(Walker *, Select *);
  int walkerDepth;
  u16 eCode;
  u16 mWFlags;
  union {
    NameContext *pNC;
    int n;
    int iCur;
    int sz;
    SrcList *pSrcList;
    struct CCurHint *pCCurHint;
    struct RefSrcList *pRefSrcList;
    int *aiCol;
    struct IdxCover *pIdxCover;
    ExprList *pGroupBy;
    Select *pSelect;
    struct WindowRewrite *pRewrite;
    struct WhereConst *pConst;
    struct RenameCtx *pRename;
    struct Table *pTab;
    struct CoveringIndexCheck *pCovIdxCk;
    SrcItem *pSrcItem;
    DbFixer *pFix;
    Mem *aMem;
    struct CheckOnCtx *pCheckOnCtx;
  } u;
};

int sqlite3WalkExpr(Walker *, Expr *);
int sqlite3WalkExprNN(Walker *, Expr *);
int sqlite3WalkExprList(Walker *, ExprList *);
int sqlite3WalkSelect(Walker *, Select *);
int sqlite3WalkSelectExpr(Walker *, Select *);
int sqlite3WalkSelectFrom(Walker *, Select *);
int sqlite3ExprWalkNoop(Walker *, Expr *);
int sqlite3SelectWalkNoop(Walker *, Select *);
int sqlite3SelectWalkFail(Walker *, Select *);
int sqlite3WalkerDepthIncrease(Walker *, Select *);
void sqlite3WalkerDepthDecrease(Walker *, Select *);
void sqlite3WalkWinDefnDummyCallback(Walker *, Select *);
void sqlite3SelectPopWith(Walker *, Select *);


#ifdef __cplusplus
}
#endif

