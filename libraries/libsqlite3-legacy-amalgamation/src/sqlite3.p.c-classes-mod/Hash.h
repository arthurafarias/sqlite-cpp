
#pragma once
#ifdef __cplusplus
extern C {
#endif

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

static void sqlite3HashInit(Hash *);
static void *sqlite3HashInsert(Hash *, const char *pKey, void *pData);
static void *sqlite3HashFind(const Hash *, const char *pKey);
static void sqlite3HashClear(Hash *);

#ifdef __cplusplus
}
#endif

