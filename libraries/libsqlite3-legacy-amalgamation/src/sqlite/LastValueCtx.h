
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/sqlite3_value.h"

typedef struct LastValueCtx LastValueCtx;

struct LastValueCtx {
  sqlite3_value *pVal;
  int nVal;
};

#ifdef __cplusplus
}
#endif
