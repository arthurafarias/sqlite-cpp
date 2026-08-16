
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/u8.h"

  typedef struct CollSeq CollSeq;

  struct CollSeq {
    char *zName;
    u8 enc;
    void *pUser;
    int (*xCmp)(void *, int, const void *, int, const void *);
    void (*xDel)(void *);
  };

  int sqlite3IsBinary(const CollSeq *);

#ifdef __cplusplus
}
#endif
