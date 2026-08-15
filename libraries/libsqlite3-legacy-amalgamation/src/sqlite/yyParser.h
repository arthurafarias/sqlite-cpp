
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/yyStackEntry.h"

typedef struct yyParser yyParser;
struct yyParser {
  yyStackEntry *yytos;

  Parse *pParse;
  yyStackEntry *yystackEnd;
  yyStackEntry *yystack;
  yyStackEntry yystk0[50];
};

#ifdef __cplusplus
}
#endif

