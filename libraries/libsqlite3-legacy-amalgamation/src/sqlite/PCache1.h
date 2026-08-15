
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct PCache1 PCache1;
struct PCache1 {

  PGroup *pGroup;
  unsigned int *pnPurgeable;
  int szPage;
  int szExtra;
  int szAlloc;
  int bPurgeable;
  unsigned int nMin;
  unsigned int nMax;
  unsigned int n90pct;
  unsigned int iMaxKey;
  unsigned int nPurgeableDummy;

  unsigned int nRecyclable;
  unsigned int nPage;
  unsigned int nHash;
  PgHdr1 **apHash;
  PgHdr1 *pFree;
  void *pBulk;
};

#ifdef __cplusplus
}
#endif

