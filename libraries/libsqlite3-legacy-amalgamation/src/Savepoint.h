
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct Savepoint Savepoint;

struct Savepoint {
  char *zName;
  i64 nDeferredCons;
  i64 nDeferredImmCons;
  Savepoint *pNext;
};

#ifdef __cplusplus
}
#endif

