
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct RenameToken RenameToken;
struct RenameToken {
  const void *p;
  Token t;
  RenameToken *pNext;
};

#ifdef __cplusplus
}
#endif

