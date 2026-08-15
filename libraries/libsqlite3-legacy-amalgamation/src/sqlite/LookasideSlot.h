
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct LookasideSlot LookasideSlot;


struct LookasideSlot {
  LookasideSlot *pNext;
};

#ifdef __cplusplus
}
#endif

