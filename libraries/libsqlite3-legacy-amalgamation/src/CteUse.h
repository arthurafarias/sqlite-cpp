
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
#include "u8.h"
#include "yDbMask.h"
#include "ynVar.h"

  typedef struct CteUse CteUse;

  struct CteUse {
    int nUse;
    int addrM9e;
    int regRtn;
    int iCur;
    LogEst nRowEst;
    u8 eM10d;
  };

#ifdef __cplusplus
}
#endif
