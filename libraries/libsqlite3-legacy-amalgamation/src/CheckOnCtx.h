
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "DbPage.h"
#include "Expr.h"
#include "InitData.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "SrcList.h"
#include "StrAccum.h"
#include "Walker.h"
#include "yDbMask.h"
#include "ynVar.h"

  typedef struct CheckOnCtx CheckOnCtx;
  struct CheckOnCtx {
    SrcList *pSrc;
    int iJoin;
    int bFuncArg;
    CheckOnCtx *pParent;
  };

#ifdef __cplusplus
}
#endif
