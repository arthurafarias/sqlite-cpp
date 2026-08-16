#include "sqlite/_All.h"

void statAccumDestructor(void *pOld) {
  StatAccum *p = (StatAccum *)pOld;

  sqlite3DbFree(p->db, p);
}
