
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WalCkptInfo WalCkptInfo;
struct WalCkptInfo {
  u32 nBackfill;
  u32 aReadMark[(8 - 3)];
  u8 aLock[8];
  u32 nBackfillAttempted;
  u32 notUsed0;
};

#ifdef __cplusplus
}
#endif

