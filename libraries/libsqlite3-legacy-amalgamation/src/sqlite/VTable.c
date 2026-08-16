#include "sqlite/_All.h"

void sqlite3VtabLock(VTable *pVTab) { pVTab->nRef++; }

void sqlite3VtabUnlock(VTable *pVTab) {
  sqlite3 *db = pVTab->db;

  pVTab->nRef--;
  if (pVTab->nRef == 0) {
    sqlite3_vtab *p = pVTab->pVtab;
    if (p) {
      p->pModule->xDisconnect(p);
    }
    sqlite3VtabModuleUnref(pVTab->db, pVTab->pMod);
    sqlite3DbFree(db, pVTab);
  }
}
