
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
typedef struct sqlite3_file sqlite3_file;
typedef struct PmaWriter PmaWriter;
struct PmaWriter {
  int eFWErr;
  u8 *aBuffer;
  int nBuffer;
  int iBufStart;
  int iBufEnd;
  i64 iWriteOff;
  sqlite3_file *pFd;
  u64 nPmaSpill;
};

#ifdef __cplusplus
}
#endif

