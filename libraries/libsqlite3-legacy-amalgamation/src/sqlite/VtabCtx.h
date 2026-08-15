
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct VtabCtx VtabCtx;
struct VtabCtx {
  VTable *pVTable;
  Table *pTab;
  VtabCtx *pPrior;
  int bDeclared;
};

#ifdef __cplusplus
}
#endif

