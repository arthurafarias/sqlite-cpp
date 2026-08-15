
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct BusyHandler BusyHandler;

  struct BusyHandler {
    int (*xBusyHandler)(void *, int);
    void *pBusyArg;
    int nBusy;
  };
  
#ifdef __cplusplus
}
#endif
