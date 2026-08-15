#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/i64.h"

#include "sqlite/CollSeq.h"
#include "sqlite/sqlite3_value.h"

typedef struct sqlite3_value Mem;

int sqlite3MemCompare(const Mem *, const Mem *, const CollSeq *);
int sqlite3BlobCompare(const Mem *, const Mem *);
i64 sqlite3VdbeIntValue(const Mem *);

#ifdef __cplusplus
}
#endif
