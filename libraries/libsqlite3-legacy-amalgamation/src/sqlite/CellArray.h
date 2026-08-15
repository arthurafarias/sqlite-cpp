
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/u8.h"
#include "sqlite/u16.h"

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
