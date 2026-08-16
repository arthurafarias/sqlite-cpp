
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/Pgno.h"

  typedef struct TableLock TableLock;
  struct TableLock {
    int iDb;
    Pgno iTab;
    u8 isWriteLock;
    const char *zLockName;
  };

#ifdef __cplusplus
}
#endif
