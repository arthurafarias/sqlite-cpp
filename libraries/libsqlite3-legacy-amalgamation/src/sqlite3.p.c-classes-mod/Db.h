
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Db Db;

struct Db {
  char *zDbSName;
  Btree *pBt;
  u8 safety_level;
  u8 bSyncSet;
  Schema *pSchema;
};

#ifdef __cplusplus
}
#endif

