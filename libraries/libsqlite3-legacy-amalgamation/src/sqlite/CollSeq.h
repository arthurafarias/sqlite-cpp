
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/u8.h"

  typedef struct CollSeq CollSeq;

  struct CollSeq {
    char *zName;
    u8 enc;
    void *pUser;
    int (*xCmp)(void *, int, const void *, int, const void *);
    void (*xDel)(void *);
  };

#ifdef __cplusplus
}
#endif
