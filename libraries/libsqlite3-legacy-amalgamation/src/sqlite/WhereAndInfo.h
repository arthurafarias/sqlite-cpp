
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/WhereClause.h"

  typedef struct WhereAndInfo WhereAndInfo;
  struct WhereAndInfo {
    WhereClause wc;
  };

#ifdef __cplusplus
}
#endif
