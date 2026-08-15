
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct RCStr RCStr;

struct RCStr {
  u64 nRCRef;
};

#ifdef __cplusplus
}
#endif

