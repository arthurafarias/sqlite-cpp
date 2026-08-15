
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct HiddenIndexInfo HiddenIndexInfo;
struct HiddenIndexInfo {
  WhereClause *pWC;
  Parse *pParse;
  int eDistinct;
  u32 mIn;
  u32 mHandleIn;
  sqlite3_value *aRhs[];
};

#ifdef __cplusplus
}
#endif

