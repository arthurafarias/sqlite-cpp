
#pragma once
#ifdef __cplusplus
extern C {
#endif

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

#ifdef __cplusplus
}
#endif

