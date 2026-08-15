
#pragma once
#ifdef __cplusplus
extern C {
#endif

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

static unixInodeInfo *inodeList = 0;

#ifdef __cplusplus
}
#endif

