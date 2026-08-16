
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/PgHdr1.h"

  typedef struct PGroup PGroup;
  struct PGroup {
    sqlite3_mutex *mutex;
    unsigned int nMaxPage;
    unsigned int nMinPage;
    unsigned int mxPinned;
    unsigned int nPurgeable;
    PgHdr1 lru;
  };

#ifdef __cplusplus
}
#endif
