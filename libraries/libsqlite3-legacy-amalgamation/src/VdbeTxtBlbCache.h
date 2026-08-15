
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct VdbeTxtBlbCache VdbeTxtBlbCache;
struct VdbeTxtBlbCache {
  char *pCValue;
  i64 iOffset;
  int iCol;
  u32 cacheStatus;
  u32 colCacheCtr;
};

#ifdef __cplusplus
}
#endif

