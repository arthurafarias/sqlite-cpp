
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/Pgno.h"

typedef struct Table Table;


struct Table {
  char *zName;
  Column *aCol;
  Index *pIndex;
  char *zColAff;
  ExprList *pCheck;

  Pgno tnum;
  u32 nTabRef;
  u32 tabFlags;
  i16 iPKey;
  i16 nCol;
  i16 nNVCol;
  LogEst nRowLogEst;
  LogEst szTabRow;

  u8 keyConf;
  u8 eTabType;
  union {
    struct {
      int addColOffset;
      FKey *pFKey;
      ExprList *pDfltList;

    } tab;
    struct {
      Select *pSelect;
    } view;
    struct {
      int nArg;
      char **azArg;
      VTable *p;
    } vtab;
  } u;
  Trigger *pTrigger;
  Schema *pSchema;
  u8 aHx[16];
};

#ifdef __cplusplus
}
#endif

