
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

typedef struct JsonPretty JsonPretty;

struct JsonPretty {
  JsonParse *pParse;
  JsonString *pOut;
  const char *zIndent;
  u32 szIndent;
  u32 nIndent;
};

#ifdef __cplusplus
}
#endif

