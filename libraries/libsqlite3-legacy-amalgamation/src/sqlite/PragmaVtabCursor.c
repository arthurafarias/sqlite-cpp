#include "sqlite/_All.h"

void pragmaVtabCursorClear(PragmaVtabCursor *pCsr) {
  int i;
  sqlite3_finalize(pCsr->pPragma);
  pCsr->pPragma = 0;
  pCsr->iRowid = 0;
  for (i = 0; i < ((int)(sizeof(pCsr->azArg) / sizeof(pCsr->azArg[0]))); i++) {
    sqlite3_free(pCsr->azArg[i]);
    pCsr->azArg[i] = 0;
  }
}
