
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/Hash.h"

typedef struct Schema Schema;

struct Schema {
  int schema_cookie;
  int iGeneration;
  Hash tblHash;
  Hash idxHash;
  Hash trigHash;
  Hash fkeyHash;
  Table *pSeqTab;
  u8 file_format;
  u8 enc;
  u16 schemaFlags;
  int cache_size;
};

#ifdef __cplusplus
}
#endif

