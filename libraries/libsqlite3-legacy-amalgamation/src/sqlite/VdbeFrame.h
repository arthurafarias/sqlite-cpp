
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/Mem.h"
#include "sqlite/Op.h"

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

  void sqlite3VdbeFrameMemDel(void *);
  void sqlite3VdbeFrameDelete(VdbeFrame *);
  int sqlite3VdbeFrameRestore(VdbeFrame *);

#ifdef __cplusplus
}
#endif
