
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct WalHashLoc WalHashLoc;
typedef struct WalHashLoc WalHashLoc;
struct WalHashLoc {
  volatile ht_slot *aHash;
  volatile u32 *aPgno;
  u32 iZero;
};

#ifdef __cplusplus
}
#endif

