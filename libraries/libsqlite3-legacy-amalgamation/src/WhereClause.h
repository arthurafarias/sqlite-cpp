
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

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

#ifdef __cplusplus
}
#endif

