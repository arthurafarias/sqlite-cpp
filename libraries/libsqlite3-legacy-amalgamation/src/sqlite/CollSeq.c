#include "sqlite/_All.h"

int sqlite3IsBinary(const CollSeq *p) { return p == 0 || p->xCmp == binCollFunc; }
