
#pragma once

#ifdef __cplusplus
extern C {
#endif

#include "sqlite/SorterFile.h"

  typedef struct IncrMerger IncrMerger;
  struct IncrMerger {
    SortSubtask *pTask;
    MergeEngine *pMerger;
    i64 iStartOff;
    int mxSz;
    int bEof;
    int bUseThread;
    SorterFile aFile[2];
  };

  int vdbeIncrSwap(IncrMerger *);
  void vdbeIncrFree(IncrMerger *);
  int vdbeIncrPopulate(IncrMerger * pIncr);
  int vdbeIncrBgPopulate(IncrMerger * pIncr);
  void vdbeIncrMergerSetThreads(IncrMerger * pIncr);

#ifdef __cplusplus
}
#endif
