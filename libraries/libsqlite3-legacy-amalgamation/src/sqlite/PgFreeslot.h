
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct PgFreeslot PgFreeslot;
  struct PgFreeslot {
    PgFreeslot *pNext;
  };

#ifdef __cplusplus
}
#endif
