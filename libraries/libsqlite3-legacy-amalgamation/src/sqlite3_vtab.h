
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct sqlite3_vtab sqlite3_vtab;
typedef struct sqlite3_module sqlite3_module;

struct sqlite3_vtab {
  const sqlite3_module *pModule;
  int nRef;
  char *zErrMsg;
};

#ifdef __cplusplus
}
#endif

