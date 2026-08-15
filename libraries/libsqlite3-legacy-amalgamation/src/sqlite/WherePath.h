
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/BitMask.h"

typedef struct WherePath WherePath;
struct WherePath {
  Bitmask maskLoop;
  Bitmask revLoop;
  LogEst nRow;
  LogEst rCost;
  LogEst rUnsort;
  i8 isOrdered;
  WhereLoop **aLoop;
};


#ifdef __cplusplus
}
#endif

