
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WhereTerm WhereTerm;
struct WhereTerm {
  Expr *pExpr;
  WhereClause *pWC;
  LogEst truthProb;
  u16 wtFlags;
  u16 eOperator;
  u8 nChild;
  u8 eMatchOp;
  int iParent;
  int leftCursor;

  union {
    struct {
      int leftColumn;
      int iField;
    } x;
    WhereOrInfo *pOrInfo;
    WhereAndInfo *pAndInfo;
  } u;
  Bitmask prereqRight;
  Bitmask prereqAll;
};


#ifdef __cplusplus
}
#endif

