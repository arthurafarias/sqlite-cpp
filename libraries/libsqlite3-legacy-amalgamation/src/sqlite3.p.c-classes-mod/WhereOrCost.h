
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct WhereOrCost WhereOrCost;
struct WhereOrCost {
  Bitmask prereq;
  LogEst rRun;
  LogEst nOut;
};



#ifdef __cplusplus
}
#endif

