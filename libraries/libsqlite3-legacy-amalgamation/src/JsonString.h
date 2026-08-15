
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct JsonString JsonString;

struct JsonString {
  sqlite3_context *pCtx;
  char *zBuf;
  u64 nAlloc;
  u64 nUsed;
  u8 bStatic;
  u8 eErr;
  char zSpace[100];
};

#ifdef __cplusplus
}
#endif

