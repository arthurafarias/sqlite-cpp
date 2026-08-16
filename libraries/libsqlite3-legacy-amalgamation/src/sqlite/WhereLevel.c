#include "sqlite/_All.h"

void disableTerm(WhereLevel *pLevel, WhereTerm *pTerm) {
  int nLoop = 0;


  while ((pTerm->wtFlags & 0x0004) == 0 && (pLevel->iLeftJoin == 0 || (((pTerm->pExpr)->flags & (u32)(0x000001)) != 0)) && (pLevel->notReady & pTerm->prereqAll) == 0) {
    if (nLoop && (pTerm->wtFlags & 0x0400) != 0) {
      pTerm->wtFlags |= 0x0200;
    } else {
      pTerm->wtFlags |= 0x0004;
    }

    if (pTerm->iParent < 0)
      break;
    pTerm = &pTerm->pWC->a[pTerm->iParent];

    ((void)(0))

        ;
    pTerm->nChild--;
    if (pTerm->nChild != 0)
      break;
    nLoop++;
  }
}
