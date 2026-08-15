
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "AggInfo.h"
#include "CteUse.h"
#include "DbPage.h"
#include "Expr.h"
#include "ExprList.h"
#include "InitData.h"
#include "Select.h"
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

  typedef struct Cte Cte;

  struct Cte {
    char *zName;
    ExprList *pCols;
    Select *pSelect;
    const char *zCteErr;
    CteUse *pUse;
    u8 eM10d;
  };

#ifdef __cplusplus
}
#endif
