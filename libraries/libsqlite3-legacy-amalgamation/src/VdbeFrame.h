
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct VdbeFrame VdbeFrame;
struct VdbeFrame {
  Vdbe *v;
  VdbeFrame *pParent;
  Op *aOp;
  Mem *aMem;
  VdbeCursor **apCsr;
  u8 *aOnce;
  void *token;
  i64 lastRowid;
  AuxData *pAuxData;

  int nCursor;
  int pc;
  int nOp;
  int nMem;
  int nChildMem;
  int nChildCsr;
  i64 nChange;
  i64 nDbChange;
};

#ifdef __cplusplus
}
#endif

