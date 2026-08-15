
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct JsonParse JsonParse;

struct JsonParse {
  u8 *aBlob;
  u32 nBlob;
  u32 nBlobAlloc;
  char *zJson;
  sqlite3 *db;
  int nJson;
  u32 nJPRef;
  u32 iErr;
  u16 iDepth;
  u8 nErr;
  u8 oom;
  u8 bJsonIsRCStr;
  u8 hasNonstd;
  u8 bReadOnly;

  u8 eEdit;
  int delta;
  u32 nIns;
  u32 iLabel;
  u8 *aIns;
};

#ifdef __cplusplus
}
#endif

