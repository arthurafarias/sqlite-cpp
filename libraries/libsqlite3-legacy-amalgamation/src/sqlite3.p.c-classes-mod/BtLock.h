
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "Pgno.h"
#include "u8.h"

typedef struct BtLock BtLock;
typedef struct Btree Btree;

struct BtLock {
  Btree *pBtree;
  Pgno iTable;
  u8 eLock;
  BtLock *pNext;
};

#ifdef __cplusplus
}
#endif

