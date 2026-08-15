
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WhereRightJoin WhereRightJoin;
struct WhereRightJoin {
  int iMatch;
  int regBloom;
  int regReturn;
  int addrSubrtn;
  int endSubrtn;
};


#ifdef __cplusplus
}
#endif

