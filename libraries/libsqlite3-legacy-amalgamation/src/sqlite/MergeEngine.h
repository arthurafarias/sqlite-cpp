
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct MergeEngine MergeEngine;
struct MergeEngine {
  int nTree;
  SortSubtask *pTask;
  int *aTree;
  PmaReader *aReadr;
};

#ifdef __cplusplus
}
#endif

