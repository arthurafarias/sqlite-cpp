
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct DblquoteStr DblquoteStr;
struct DblquoteStr {
  DblquoteStr *pNextStr;
  char z[8];
};

#ifdef __cplusplus
}
#endif

