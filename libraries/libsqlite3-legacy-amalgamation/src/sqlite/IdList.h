
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct IdList IdList;


struct IdList {
  int nId;
  struct IdList_item {
    char *zName;
  } a[];
};

#ifdef __cplusplus
}
#endif

