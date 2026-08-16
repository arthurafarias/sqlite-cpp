
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct KeyInfo KeyInfo;

  struct KeyInfo {
    u32 nRef;
    u8 enc;
    u16 nKeyField;
    u16 nAllField;
    sqlite3 *db;
    u8 *aSortFlags;
    CollSeq *aColl[];
  };

  UnpackedRecord *sqlite3VdbeAllocUnpackedRecord(KeyInfo *);
  void sqlite3KeyInfoUnref(KeyInfo *);
  KeyInfo *sqlite3KeyInfoRef(KeyInfo *);

#ifdef __cplusplus
}
#endif
