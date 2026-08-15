
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "DbPage.h"
#include "InitData.h"
#include "MemPage.h"
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

  typedef struct CellArray CellArray;
  struct CellArray {
    int nCell;
    MemPage *pRef;
    u8 **apCell;
    u16 *szCell;
    u8 *apEnd[3 * 2];
    int ixNx[3 * 2];
  };

#ifdef __cplusplus
}
#endif
