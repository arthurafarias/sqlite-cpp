
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/Mem.h"

  typedef struct UnpackedRecord UnpackedRecord;

  struct UnpackedRecord {
    KeyInfo *pKeyInfo;
    Mem *aMem;
    union {
      char *z;
      i64 i;
    } u;
    int n;
    u16 nField;
    i8 default_rc;
    u8 errCode;
    i8 r1;
    i8 r2;
    u8 eqSeen;
  };

  RecordCompare sqlite3VdbeFindCompare(UnpackedRecord *);

#ifdef __cplusplus
}
#endif
