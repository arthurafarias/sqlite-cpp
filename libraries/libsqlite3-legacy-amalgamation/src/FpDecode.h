
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

typedef struct FpDecode FpDecode;

struct FpDecode {
  int n;
  int iDP;
  char *z;
  char zBuf[20 + 1];
  char sign;
  char isSpecial;
};

#ifdef __cplusplus
}
#endif

