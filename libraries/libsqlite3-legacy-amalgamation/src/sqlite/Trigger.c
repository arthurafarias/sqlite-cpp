#include "sqlite/_All.h"

Table *tableOfTrigger(Trigger *pTrigger) { return sqlite3HashFind(&pTrigger->pTabSchema->tblHash, pTrigger->table); }
