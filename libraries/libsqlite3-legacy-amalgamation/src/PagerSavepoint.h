
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct PagerSavepoint PagerSavepoint;
struct PagerSavepoint {
  i64 iOffset;
  i64 iHdrOffset;
  Bitvec *pInSavepoint;
  Pgno nOrig;
  Pgno iSubRec;
  int bTruncateOnRelease;

  u32 aWalData[4];
};

#ifdef __cplusplus
}
#endif

