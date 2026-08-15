
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct sqlite3StatType sqlite3StatType;

struct sqlite3StatType {
  sqlite3StatValueType nowValue[10];
  sqlite3StatValueType mxValue[10];
} sqlite3Stat = {{
                     0,
                 },
                 {
                     0,
                 }};

#ifdef __cplusplus
}
#endif

