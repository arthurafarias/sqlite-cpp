
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct RenameCtx RenameCtx;
struct RenameCtx {
  RenameToken *pList;
  int nList;
  int iCol;
  Table *pTab;
  const char *zOld;
};

#ifdef __cplusplus
}
#endif

