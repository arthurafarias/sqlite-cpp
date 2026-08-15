
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct unixFile unixFile;
struct unixFile {
  sqlite3_io_methods const *pMethod;
  sqlite3_vfs *pVfs;
  unixInodeInfo *pInode;
  int h;
  unsigned char eFileLock;
  unsigned short int ctrlFlags;
  int lastErrno;
  void *lockingContext;
  UnixUnusedFd *pPreallocatedUnused;
  const char *zPath;
  unixShm *pShm;
  int szChunk;

  int nFetchOut;
  sqlite3_int64 mmapSize;
  sqlite3_int64 mmapSizeActual;
  sqlite3_int64 mmapSizeMax;
  void *pMapRegion;

  int sectorSize;
  int deviceCharacteristics;
};

#ifdef __cplusplus
}
#endif

