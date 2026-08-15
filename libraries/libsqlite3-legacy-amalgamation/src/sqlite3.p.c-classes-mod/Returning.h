
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Returning Returning;


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

