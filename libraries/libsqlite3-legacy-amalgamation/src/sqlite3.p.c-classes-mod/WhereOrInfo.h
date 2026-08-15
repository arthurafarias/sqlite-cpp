
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct WhereOrInfo WhereOrInfo;
struct WhereOrInfo {
  WhereClause wc;
  Bitmask indexable;
};

#ifdef __cplusplus
}
#endif

