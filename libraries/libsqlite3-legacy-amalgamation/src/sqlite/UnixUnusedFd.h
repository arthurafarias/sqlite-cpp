
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct UnixUnusedFd UnixUnusedFd;

struct UnixUnusedFd {
  int fd;
  int flags;
  UnixUnusedFd *pNext;
};


#ifdef __cplusplus
}
#endif

