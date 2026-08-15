
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct IndexIterator IndexIterator;
struct IndexIterator {
  int eType;
  int i;
  union {
    struct {
      Index *pIdx;
    } lx;
    struct {
      int nIdx;
      IndexListTerm *aIdx;
    } ax;
  } u;
};


#ifdef __cplusplus
}
#endif

