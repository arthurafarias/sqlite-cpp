
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/WhereClause.h"
#include "sqlite/BitMask.h"

typedef struct WhereOrInfo WhereOrInfo;
struct WhereOrInfo {
  WhereClause wc;
  Bitmask indexable;
};

#ifdef __cplusplus
}
#endif

