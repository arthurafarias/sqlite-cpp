
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Expr Expr;


struct Expr {
  u8 op;
  char affExpr;
  u8 op2;

  u32 flags;
  union {
    char *zToken;
    int iValue;
  } u;

  Expr *pLeft;
  Expr *pRight;
  union {
    ExprList *pList;
    Select *pSelect;
  } x;

  int nHeight;

  int iTable;

  ynVar iColumn;

  i16 iAgg;
  union {
    int iJoin;
    int iOfst;
  } w;
  AggInfo *pAggInfo;
  union {
    Table *pTab;

    Window *pWin;
    int nReg;
    struct {
      int iAddr;
      int regReturn;
    } sub;
  } y;
};

#ifdef __cplusplus
}
#endif

