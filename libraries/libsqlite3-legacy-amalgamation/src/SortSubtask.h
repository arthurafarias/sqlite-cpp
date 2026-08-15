
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct SortSubtask SortSubtask;
struct SortSubtask {
  SQLiteThread *pThread;
  int bDone;
  int nPMA;
  VdbeSorter *pSorter;
  UnpackedRecord *pUnpacked;
  SorterList list;
  SorterCompare xCompare;
  SorterFile file;
  SorterFile file2;
  u64 nSpill;
};


#ifdef __cplusplus
}
#endif

