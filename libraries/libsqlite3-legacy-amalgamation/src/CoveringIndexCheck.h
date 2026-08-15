
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "DbPage.h"
#include "Expr.h"
#include "Index.h"
#include "InitData.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "StrAccum.h"
#include "u8.h"
#include "Walker.h"
#include "yDbMask.h"
#include "ynVar.h"

  typedef struct CoveringIndexCheck CoveringIndexCheck;
  struct CoveringIndexCheck {
    Index *pIdx;
    int iTabCur;
    u8 bExpr;
    u8 bUnidx;
  };

#ifdef __cplusplus
}
#endif
