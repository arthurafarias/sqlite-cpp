
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Window Window;


struct Window {
  char *zName;
  char *zBase;
  ExprList *pPartition;
  ExprList *pOrderBy;
  u8 eFrmType;
  u8 eStart;
  u8 eEnd;
  u8 bImplicitFrame;
  u8 eExclude;
  Expr *pStart;
  Expr *pEnd;
  Window **ppThis;
  Window *pNextWin;
  Expr *pFilter;
  FuncDef *pWFunc;
  int iEphCsr;
  int regAccum;
  int regResult;
  int csrApp;
  int regApp;
  int regPart;
  Expr *pOwner;
  int nBufferCol;
  int iArgCol;
  int regOne;
  int regStartRowid;
  int regEndRowid;
  u8 bExprArgs;
};

#ifdef __cplusplus
}
#endif

