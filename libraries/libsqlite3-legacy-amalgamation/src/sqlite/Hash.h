
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

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

