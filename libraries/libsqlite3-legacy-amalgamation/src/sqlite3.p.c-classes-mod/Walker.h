
#pragma once
#ifdef __cplusplus
extern C {
#endif

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


#ifdef __cplusplus
}
#endif

