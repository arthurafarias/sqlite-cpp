
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

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

#ifdef __cplusplus
}
#endif

