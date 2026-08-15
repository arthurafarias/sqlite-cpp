
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WhereMaskSet WhereMaskSet;
struct WhereMaskSet {
  int bVarSelect;
  int n;
  int ix[((int)(sizeof(Bitmask) * 8))];
};

#ifdef __cplusplus
}
#endif

