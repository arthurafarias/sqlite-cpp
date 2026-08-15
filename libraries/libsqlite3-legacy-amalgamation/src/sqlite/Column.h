
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/u8.h"
#include "sqlite/u16.h"

  typedef struct Column Column;

  struct Column {
    char *zCnName;
    unsigned notNull : 4;
    unsigned eCType : 4;
    char affinity;
    u8 szEst;
    u8 hName;
    u16 iDflt;
    u16 colFlags;
  };

#ifdef __cplusplus
}
#endif
