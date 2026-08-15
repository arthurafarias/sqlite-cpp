
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct IndexListTerm IndexListTerm;
struct IndexListTerm {
  Index *p;
  int ix;
};

#ifdef __cplusplus
}
#endif

