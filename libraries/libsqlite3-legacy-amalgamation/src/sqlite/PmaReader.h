
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct PmaReader PmaReader;
struct PmaReader {
  i64 iReadOff;
  i64 iEof;
  int nAlloc;
  int nKey;
  sqlite3_file *pFd;
  u8 *aAlloc;
  u8 *aKey;
  u8 *aBuffer;
  int nBuffer;
  u8 *aMap;
  IncrMerger *pIncr;
};



#ifdef __cplusplus
}
#endif

