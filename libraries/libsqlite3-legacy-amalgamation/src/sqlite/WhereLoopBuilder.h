
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct WhereLoopBuilder WhereLoopBuilder;

struct WhereLoopBuilder {
  WhereInfo *pWInfo;
  WhereClause *pWC;
  WhereLoop *pNew;
  WhereOrSet *pOrSet;

  unsigned char bldFlags1;
  unsigned char bldFlags2;
  unsigned int iPlanLimit;
};


#ifdef __cplusplus
}
#endif

