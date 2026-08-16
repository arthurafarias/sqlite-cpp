
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/unixFileId.h"

  typedef struct unixInodeInfo unixInodeInfo;

  struct unixInodeInfo {
    struct unixFileId fileId;
    sqlite3_mutex *pLockMutex;
    int nShared;
    int nLock;
    unsigned char eFileLock;
    unsigned char bProcessLock;
    UnixUnusedFd *pUnused;
    int nRef;
    unixShmNode *pShmNode;
    unixInodeInfo *pNext;
    unixInodeInfo *pPrev;
  };

  extern unixInodeInfo *inodeList;

#ifdef __cplusplus
}
#endif
