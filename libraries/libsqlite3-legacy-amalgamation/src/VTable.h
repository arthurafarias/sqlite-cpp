
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct VTable VTable;


struct VTable {
  sqlite3 *db;
  Module *pMod;
  sqlite3_vtab *pVtab;
  int nRef;
  u8 bConstraint;
  u8 bAllSchemas;
  u8 eVtabRisk;
  int iSavepoint;
  VTable *pNext;
};

#ifdef __cplusplus
}
#endif

