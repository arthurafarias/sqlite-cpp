
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/DbPage.h"
#include "sqlite/sqlite3_file.h"
#include "sqlite/sqlite3_hard_heap.h"
#include "sqlite/sqlite3_libversion.h"
#include "sqlite/sqlite3_libversion_number.h"
#include "sqlite/sqlite3_soft_heap.h"
#include "sqlite/sqlite3_sourceid.h"
#include "sqlite/StrAccum.h"
#include "sqlite/yDbMask.h"
#include "sqlite/ynVar.h"

typedef struct DbPath DbPath;
struct DbPath {
  int rc;
  int nSymlink;
  char *zOut;
  int nOut;
  int nUsed;
};

#ifdef __cplusplus
}
#endif

