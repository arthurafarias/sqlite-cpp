
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "DbPage.h"
#include "InitData.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "StrAccum.h"
#include "yDbMask.h"
#include "ynVar.h"

typedef struct HashElem HashElem;

struct HashElem {
  HashElem *next, *prev;
  void *data;
  const char *pKey;
  unsigned int h;
};

#ifdef __cplusplus
}
#endif

