
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

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

