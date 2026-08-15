
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct With With;


struct With {
  int nCte;
  int bView;
  With *pOuter;
  Cte a[];
};

#ifdef __cplusplus
}
#endif

