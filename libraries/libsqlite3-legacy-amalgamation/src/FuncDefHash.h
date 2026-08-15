
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "AggInfo.h"
#include "DbPage.h"
#include "FuncDef.h"
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

typedef struct FuncDefHash FuncDefHash;

struct FuncDefHash {
  FuncDef *a[23];
};

#ifdef __cplusplus
}
#endif

