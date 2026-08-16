
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct FuncDef FuncDef;
typedef struct sqlite3_value sqlite3_value;

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

