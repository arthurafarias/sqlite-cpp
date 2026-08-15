
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/sqlite3_vtab.h"
#include "sqlite/PragmaName.h"

typedef struct PragmaVtab PragmaVtab;
struct PragmaVtab {
  sqlite3_vtab base;
  sqlite3 *db;
  const PragmaName *pName;
  u8 nHidden;
  u8 iHidden;
};

#ifdef __cplusplus
}
#endif

