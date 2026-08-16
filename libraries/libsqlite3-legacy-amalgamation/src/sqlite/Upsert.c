#include "sqlite/_All.h"

int sqlite3UpsertNextIsIPK(Upsert *pUpsert) {
  Upsert *pNext;
  if ((pUpsert == 0))
    return 0;
  pNext = pUpsert->pNextUpsert;
  while (1) {
    if (pNext == 0)
      return 1;
    if (pNext->pUpsertTarget == 0)
      return 1;
    if (pNext->pUpsertIdx == 0)
      return 1;
    if (!pNext->isDup)
      return 0;
    pNext = pNext->pNextUpsert;
  }
  return 0;
}

Upsert *sqlite3UpsertOfIndex(Upsert *pUpsert, Index *pIdx) {
  while (pUpsert && pUpsert->pUpsertTarget != 0 && pUpsert->pUpsertIdx != pIdx) {
    pUpsert = pUpsert->pNextUpsert;
  }
  return pUpsert;
}
