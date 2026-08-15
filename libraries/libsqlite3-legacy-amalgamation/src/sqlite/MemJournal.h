
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"
#include "sqlite/FilePoint.h"

typedef struct MemJournal MemJournal;

struct MemJournal {
  const sqlite3_io_methods *pMethod;
  int nChunkSize;

  int nSpill;
  FileChunk *pFirst;
  FilePoint endpoint;
  FilePoint readpoint;

  int flags;
  sqlite3_vfs *pVfs;
  const char *zJournal;
};

#ifdef __cplusplus
}
#endif

