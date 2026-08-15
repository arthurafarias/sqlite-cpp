
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/u8.h"

#include "sqlite/Walker.h"

  typedef struct DbFixer DbFixer;
  typedef struct Parse Parse;
  typedef struct Walker Walker;
  typedef struct Schema Schema;
  typedef struct Token Token;

  struct DbFixer {
    Parse *pParse;
    Walker w;
    Schema *pSchema;
    u8 bTemp;
    const char *zDb;
    const char *zType;
    const Token *pName;
  };

#ifdef __cplusplus
}
#endif
