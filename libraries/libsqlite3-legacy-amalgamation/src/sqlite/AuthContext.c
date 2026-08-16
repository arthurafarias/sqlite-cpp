#include "sqlite/_All.h"

void sqlite3AuthContextPop(AuthContext *pContext) {
  if (pContext->pParse) {
    pContext->pParse->zAuthContext = pContext->zAuthContext;
    pContext->pParse = 0;
  }
}
