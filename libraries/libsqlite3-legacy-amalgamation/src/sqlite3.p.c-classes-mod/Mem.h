#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i64.h"

#include "CollSeq.h"

typedef struct sqlite3_value Mem;

static int sqlite3MemCompare(const Mem *, const Mem *, const CollSeq *);
static int sqlite3BlobCompare(const Mem *, const Mem *);
static i64 sqlite3VdbeIntValue(const Mem *);

#ifdef __cplusplus
}
#endif
