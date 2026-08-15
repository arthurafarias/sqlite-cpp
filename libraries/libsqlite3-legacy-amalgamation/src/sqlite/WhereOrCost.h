
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/BitMask.h"

typedef struct WhereOrCost WhereOrCost;
struct WhereOrCost {
  Bitmask prereq;
  LogEst rRun;
  LogEst nOut;
};



#ifdef __cplusplus
}
#endif

