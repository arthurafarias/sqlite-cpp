
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

#include "DbPage.h"
#include "HashElem.h"
#include "InitData.h"
#include "sqlite3_file.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "StrAccum.h"
#include "yDbMask.h"
#include "ynVar.h"

#include "HashElem.h"

typedef struct Hash Hash;
struct Hash {
  unsigned int htsize;
  unsigned int count;
  HashElem *first;
  struct _ht {
    unsigned int count;
    HashElem *chain;
  } *ht;
};

void sqlite3HashInit(Hash *);
void *sqlite3HashInsert(Hash *, const char *pKey, void *pData);
void *sqlite3HashFind(const Hash *, const char *pKey);
void sqlite3HashClear(Hash *);

#ifdef __cplusplus
}
#endif

