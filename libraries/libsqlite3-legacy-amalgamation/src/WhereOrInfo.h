
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WhereOrInfo WhereOrInfo;
struct WhereOrInfo {
  WhereClause wc;
  Bitmask indexable;
};

#ifdef __cplusplus
}
#endif

