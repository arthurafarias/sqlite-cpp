
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct WhereConst WhereConst;
struct WhereConst {
  Parse *pParse;
  u8 *pOomFault;
  int nConst;
  int nChng;
  int bHasAffBlob;
  u32 mExcludeOn;

  Expr **apExpr;
};


#ifdef __cplusplus
}
#endif

