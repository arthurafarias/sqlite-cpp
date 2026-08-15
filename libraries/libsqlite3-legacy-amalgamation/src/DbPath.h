
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "DbPage.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "StrAccum.h"
#include "yDbMask.h"
#include "ynVar.h"

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

