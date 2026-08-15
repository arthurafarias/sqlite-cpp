
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct Returning Returning;

#include "sqlite/Trigger.h"

struct Returning {
  Parse *pParse;
  ExprList *pReturnEL;
  Trigger retTrig;
  TriggerStep retTStep;
  int iRetCur;
  int nRetCol;
  int iRetReg;
  char zName[40];
};

#ifdef __cplusplus
}
#endif

