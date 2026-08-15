
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/SrcItem.h"

typedef struct SrcList SrcList;


struct SrcList {
  int nSrc;
  u32 nAlloc;
  SrcItem a[];
};

#ifdef __cplusplus
}
#endif

