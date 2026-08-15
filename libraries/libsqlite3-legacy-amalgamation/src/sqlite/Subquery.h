
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct Subquery Subquery;

struct Subquery {
  Select *pSelect;
  int addrFillSub;
  int regReturn;
  int regResult;
};

#ifdef __cplusplus
}
#endif

