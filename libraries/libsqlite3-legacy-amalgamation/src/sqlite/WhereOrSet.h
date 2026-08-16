
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/WhereOrCost.h"
#include "sqlite/_TypeIndex.h"

  typedef struct WhereOrSet WhereOrSet;
  struct WhereOrSet {
    u16 n;
    WhereOrCost a[3];
  };

  void whereOrMove(WhereOrSet * pDest, WhereOrSet * pSrc);
  int whereOrInsert(WhereOrSet * pSet, Bitmask prereq, LogEst rRun, LogEst nOut);

#ifdef __cplusplus
}
#endif
