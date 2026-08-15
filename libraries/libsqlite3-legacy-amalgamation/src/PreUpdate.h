
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct PreUpdate PreUpdate;

struct PreUpdate {
  Vdbe *v;
  VdbeCursor *pCsr;
  int op;
  u8 *aRecord;
  KeyInfo *pKeyinfo;
  UnpackedRecord *pUnpacked;
  UnpackedRecord *pNewUnpacked;
  int iNewReg;
  int iBlobWrite;
  i64 iKey1;
  i64 iKey2;
  Mem oldipk;
  Mem *aNew;
  Table *pTab;
  Index *pPk;
  sqlite3_value **apDflt;
  struct {
    u8 keyinfoSpace[sizeof(KeyInfo)];
  } uKey;
};

#ifdef __cplusplus
}
#endif

