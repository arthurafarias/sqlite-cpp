
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WhereOrSet WhereOrSet;
struct WhereOrSet {
  u16 n;
  WhereOrCost a[3];
};


#ifdef __cplusplus
}
#endif

