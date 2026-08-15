
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct StatSample StatSample;
  struct StatSample {
    tRowcnt *anDLt;
  };

#ifdef __cplusplus
}
#endif
