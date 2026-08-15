
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct SorterList SorterList;

  struct SorterList {
    SorterRecord *pList;
    u8 *aMemory;
    i64 szPMA;
  };

#ifdef __cplusplus
}
#endif
