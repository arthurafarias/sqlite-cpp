
#pragma once
#ifdef __cplusplus
extern C {
#endif

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

