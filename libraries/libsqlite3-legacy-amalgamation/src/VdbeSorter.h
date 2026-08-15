
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

struct VdbeSorter {
  int mnPmaSize;
  int mxPmaSize;
  int mxKeysize;
  int pgsz;
  PmaReader *pReader;
  MergeEngine *pMerger;
  sqlite3 *db;
  KeyInfo *pKeyInfo;
  UnpackedRecord *pUnpacked;
  SorterList list;
  int iMemory;
  int nMemory;
  u8 bUsePMA;
  u8 bUseThreads;
  u8 iPrev;
  u8 nTask;
  u8 typeMask;
  SortSubtask aTask[];
};



#ifdef __cplusplus
}
#endif

