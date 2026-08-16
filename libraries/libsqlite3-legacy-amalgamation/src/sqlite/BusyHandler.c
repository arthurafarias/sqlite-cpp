#include "sqlite/_All.h"

int sqlite3InvokeBusyHandler(BusyHandler *p) {
  int rc;
  if (p->xBusyHandler == 0 || p->nBusy < 0)
    return 0;
  rc = p->xBusyHandler(p->pBusyArg, p->nBusy);
  if (rc == 0) {
    p->nBusy = -1;
  } else {
    p->nBusy++;
  }
  return rc;
}
