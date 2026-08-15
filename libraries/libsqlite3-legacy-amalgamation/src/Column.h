
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "DbPage.h"
#include "InitData.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "StrAccum.h"
#include "u16.h"
#include "u8.h"
#include "yDbMask.h"
#include "ynVar.h"

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
