
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct SortCtx SortCtx;
struct SortCtx {
  ExprList *pOrderBy;
  int nOBSat;
  int iECursor;
  int regReturn;
  int labelBkOut;
  int addrSortIndex;
  int labelDone;
  int labelOBLopt;
  u8 sortFlags;
  struct RowLoadInfo *pDeferredRowLoad;
};


#ifdef __cplusplus
}
#endif

