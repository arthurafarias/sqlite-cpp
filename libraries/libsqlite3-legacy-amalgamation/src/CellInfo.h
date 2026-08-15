
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "i64.h"
#include "u16.h"
#include "u32.h"
#include "u8.h"

  typedef struct CellInfo CellInfo;

  struct CellInfo {
    i64 nKey;
    u8 *pPayload;
    u32 nPayload;
    u16 nLocal;
    u16 nSize;
  };

#ifdef __cplusplus
}
#endif
