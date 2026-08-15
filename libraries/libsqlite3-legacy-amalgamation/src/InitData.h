#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Pgno.h"
#include "u32.h"

typedef struct sqlite3 sqlite3;
typedef struct InitData InitData;

struct InitData {
  sqlite3 *db;
  char **pzErrMsg;
  int iDb;
  int rc;
  u32 mInitFlags;
  u32 nInitRow;
  Pgno mxPage;
};

#ifdef __cplusplus
}
#endif