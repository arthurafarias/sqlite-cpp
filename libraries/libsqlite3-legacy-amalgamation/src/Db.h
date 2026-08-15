
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "BtLock.h"
#include "Btree.h"
#include "DbPage.h"
#include "InitData.h"
#include "Schema.h"
#include "sqlite3_backup.h"
#include "sqlite3_file.h"
#include "sqlite3.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "StrAccum.h"
#include "u8.h"
#include "yDbMask.h"
#include "ynVar.h"

  typedef struct Db Db;

  struct Db {
    char *zDbSName;
    Btree *pBt;
    u8 safety_level;
    u8 bSyncSet;
    Schema *pSchema;
  };

#ifdef __cplusplus
}
#endif
