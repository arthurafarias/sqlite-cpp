
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct OnOrUsing OnOrUsing;

struct OnOrUsing {
  Expr *pOn;
  IdList *pUsing;
};

#ifdef __cplusplus
}
#endif

