
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct OnOrUsing OnOrUsing;

struct OnOrUsing {
  Expr *pOn;
  IdList *pUsing;
};

#ifdef __cplusplus
}
#endif

