#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i64.h"

#include "CollSeq.h"

typedef struct sqlite3_value Mem;

int sqlite3MemCompare(const Mem *, const Mem *, const CollSeq *);
int sqlite3BlobCompare(const Mem *, const Mem *);
i64 sqlite3VdbeIntValue(const Mem *);

#ifdef __cplusplus
}
#endif
