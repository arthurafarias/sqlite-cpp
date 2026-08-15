
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "AggInfo.h"
#include "DbPage.h"
#include "fts5_extension_function.h"
#include "FuncDestructor.h"
#include "i16.h"
#include "InitData.h"
#include "sqlite3_context.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_module.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "sqlite3_value.h"
#include "StrAccum.h"
#include "u32.h"
#include "Vdbe.h"
#include "yDbMask.h"
#include "ynVar.h"

typedef struct FuncDef FuncDef;

struct FuncDef {
  i16 nArg;
  u32 funcFlags;
  void *pUserData;
  FuncDef *pNext;
  void (*xSFunc)(sqlite3_context *, int, sqlite3_value **);
  void (*xFinalize)(sqlite3_context *);
  void (*xValue)(sqlite3_context *);
  void (*xInverse)(sqlite3_context *, int, sqlite3_value **);
  const char *zName;
  union {
    FuncDef *pHash;
    FuncDestructor *pDestructor;
  } u;
};

#ifdef __cplusplus
}
#endif

