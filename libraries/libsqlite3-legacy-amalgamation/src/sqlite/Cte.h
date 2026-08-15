
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/u8.h"

  typedef struct Cte Cte;

  struct Cte {
    char *zName;
    ExprList *pCols;
    Select *pSelect;
    const char *zCteErr;
    CteUse *pUse;
    u8 eM10d;
  };

#ifdef __cplusplus
}
#endif
