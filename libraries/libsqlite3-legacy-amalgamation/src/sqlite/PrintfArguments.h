
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct PrintfArguments PrintfArguments;

struct PrintfArguments {
  int nArg;
  int nUsed;
  sqlite3_value **apArg;
};

#ifdef __cplusplus
}
#endif

