
#pragma once
#ifdef __cplusplus
extern C {
#endif

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

