
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct sqlite3AutoExtList sqlite3AutoExtList;
struct sqlite3AutoExtList {
  u32 nExt;
  void (**aExt)(void);
} sqlite3Autoext = {0, 0};

#ifdef __cplusplus
}
#endif

