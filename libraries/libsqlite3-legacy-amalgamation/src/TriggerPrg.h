
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct TriggerPrg TriggerPrg;


struct TriggerPrg {
  Trigger *pTrigger;
  TriggerPrg *pNext;
  SubProgram *pProgram;
  int orconf;
  u32 aColmask[2];
};

#ifdef __cplusplus
}
#endif

