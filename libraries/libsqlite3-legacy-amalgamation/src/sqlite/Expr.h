
#pragma once

#ifdef __cplusplus
extern C {
#endif

#include "sqlite/u8.h"
#include "sqlite/u32.h"
#include "sqlite/i16.h"
#include "sqlite/AggInfo.h"
#include "sqlite/ynVar.h"

typedef struct ExprList ExprList;
typedef struct Select Select;

typedef struct Expr Expr;

typedef struct Window Window;
typedef struct Index Index;
typedef struct SrcList SrcList;

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

void sqlite3DequoteExpr(Expr *);
Expr *sqlite3ExprSimplifiedAndOr(Expr *);
void sqlite3ExprToRegister(Expr *pExpr, int iReg);
int sqlite3ExprCompareSkip(Expr *, Expr *, int);
int sqlite3ExprImpliesNonNullRow(Expr *, int, int);
int sqlite3ExprCoveredByIndex(Expr *, int iCur, Index *pIdx);
int sqlite3ExprIdToTrueFalse(Expr *);
int sqlite3ExprIsConstantOrFunction(Expr *, u8);
int sqlite3ExprIsSingleTableConstraint(Expr *, const SrcList *, int, int);
int sqlite3ExprReferencesUpdatedColumn(Expr *, int *, int);
void sqlite3SetJoinExpr(Expr *, int, u32);
Expr *sqlite3ExprSkipCollate(Expr *);
Expr *sqlite3ExprSkipCollateAndLikely(Expr *);

#ifdef __cplusplus
}
#endif

