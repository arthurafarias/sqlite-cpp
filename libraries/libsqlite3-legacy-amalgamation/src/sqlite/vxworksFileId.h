
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct vxworksFileId vxworksFileId;

struct vxworksFileId {
  struct vxworksFileId *pNext;
  int nRef;
  int nName;
  char *zCanonicalName;
};

#ifdef __cplusplus
}
#endif
