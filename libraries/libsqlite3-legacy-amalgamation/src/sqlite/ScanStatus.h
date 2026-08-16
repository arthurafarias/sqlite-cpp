
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct ScanStatus ScanStatus;
  struct ScanStatus {
    int addrExplain;
    int aAddrRange[6];
    int addrLoop;
    int addrVisit;
    int iSelectID;
    LogEst nEst;
    char *zName;
  };

#ifdef __cplusplus
}
#endif
