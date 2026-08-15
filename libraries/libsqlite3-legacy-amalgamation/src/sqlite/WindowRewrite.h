
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct WindowRewrite WindowRewrite;
struct WindowRewrite {
  Window *pWin;
  SrcList *pSrc;
  ExprList *pSub;
  Table *pTab;
  Select *pSubSelect;
};

#ifdef __cplusplus
}
#endif

