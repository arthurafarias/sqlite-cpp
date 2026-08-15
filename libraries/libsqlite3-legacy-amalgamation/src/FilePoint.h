
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite3_int64.h"
#include "FileChunk.h"

typedef struct FilePoint FilePoint;

struct FilePoint {
  sqlite3_int64 iOffset;
  FileChunk *pChunk;
};


#ifdef __cplusplus
}
#endif

