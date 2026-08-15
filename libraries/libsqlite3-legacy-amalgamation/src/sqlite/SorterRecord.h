
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct SorterRecord SorterRecord;

  struct SorterRecord {
    int nVal;
    union {
      SorterRecord *pNext;
      int iNext;
    } u;
  };

#ifdef __cplusplus
}
#endif
