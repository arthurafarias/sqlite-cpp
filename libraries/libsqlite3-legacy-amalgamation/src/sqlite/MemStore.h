
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct MemStore MemStore;
struct MemStore {
  sqlite3_int64 sz;
  sqlite3_int64 szAlloc;
  sqlite3_int64 szMax;
  unsigned char *aData;
  sqlite3_mutex *pMutex;
  int nMmap;
  unsigned mFlags;
  int nRdLock;
  int nWrLock;
  int nRef;
  char *zFName;
};

#ifdef __cplusplus
}
#endif

