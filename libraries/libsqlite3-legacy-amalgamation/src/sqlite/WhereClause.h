
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/WhereTerm.h"
#include "sqlite/_TypeIndex.h"
  typedef struct WhereClause WhereClause;
  struct WhereClause {
    WhereInfo *pWInfo;
    WhereClause *pOuter;
    u8 op;
    u8 hasOr;
    int nTerm;
    int nSlot;
    int nBase;
    WhereTerm *a;

    WhereTerm aStatic[8];
  };

  WhereTerm *sqlite3WhereFindTerm(WhereClause * pWC, int iCur, int iColumn, Bitmask notReady, u32 op, Index *pIdx);
  void sqlite3WhereClauseInit(WhereClause *, WhereInfo *);
  void sqlite3WhereClauseClear(WhereClause *);
  void sqlite3WhereSplit(WhereClause *, Expr *, u8);
  void sqlite3WhereAddLimit(WhereClause *, Select *);
  int whereClauseInsert(WhereClause * pWC, Expr * p, u16 wtFlags);
  void markTermAsChild(WhereClause * pWC, int iChild, int iParent);
  void whereAddLimitExpr(WhereClause * pWC, int iReg, Expr *pExpr, int iCsr, int eMatchOp);
  WhereTerm *termFromWhereClause(WhereClause * pWC, int iTerm);
  void whereLoopOutputAdjust(WhereClause * pWC, WhereLoop * pLoop, LogEst nRow);

#ifdef __cplusplus
}
#endif
