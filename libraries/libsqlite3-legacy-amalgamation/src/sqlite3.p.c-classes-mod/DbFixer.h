
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct DbFixer DbFixer;


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

