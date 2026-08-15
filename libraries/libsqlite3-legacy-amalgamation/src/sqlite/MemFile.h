
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/sqlite3_file.h"
#include "sqlite/MemStore.h"

typedef struct MemFile MemFile;

struct MemFile {
  sqlite3_file base;
  MemStore *pStore;
  int eLock;
};

#ifdef __cplusplus
}
#endif

