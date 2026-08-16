
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/BitMask.h"
#include "sqlite/_TypeIndex.h"

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
