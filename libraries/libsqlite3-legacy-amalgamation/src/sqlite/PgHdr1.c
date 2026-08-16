#include "sqlite/_All.h"

void pcache1FreePage(PgHdr1 *p) {
  PCache1 *pCache;


  pCache = p->pCache;


  if (p->isBulkLocal) {
    p->pNext = pCache->pFree;
    pCache->pFree = p;
  } else {
    pcache1Free(p->page.pBuf);
  }
  (*pCache->pnPurgeable)--;
}

PgHdr1 *pcache1PinPage(PgHdr1 *pPage) {










  pPage->pLruPrev->pLruNext = pPage->pLruNext;
  pPage->pLruNext->pLruPrev = pPage->pLruPrev;
  pPage->pLruNext = 0;




  pPage->pCache->nRecyclable--;
  return pPage;
}

void pcache1RemoveFromHash(PgHdr1 *pPage, int freeFlag) {
  unsigned int h;
  PCache1 *pCache = pPage->pCache;
  PgHdr1 **pp;


  h = pPage->iKey % pCache->nHash;
  for (pp = &pCache->apHash[h]; (*pp) != pPage; pp = &(*pp)->pNext)
    ;
  *pp = (*pp)->pNext;

  pCache->nPage--;
  if (freeFlag)
    pcache1FreePage(pPage);
}
