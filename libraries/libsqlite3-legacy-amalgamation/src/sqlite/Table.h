
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

  Expr *sqlite3ColumnExpr(Table *, Column *);
  Index *sqlite3PrimaryKeyIndex(Table *);
  i16 sqlite3TableColumnToStorage(Table *, i16);
  i16 sqlite3StorageColumnToTable(Table *, i16);
  const char *sqlite3RowidAlias(Table * pTab);
  int sqlite3ColumnIndex(Table * pTab, const char *zCol);
  char sqlite3TableColumnAffinity(const Table *, int);
  FKey *sqlite3FkReferences(Table *);
  void estimateTableWidth(Table * pTab);
  int fkChildIsModified(Table * pTab, FKey * p, int *aChange, int bChngRowid);
  int fkParentIsModified(Table * pTab, FKey * p, int *aChange, int bChngRowid);
  int tableSkipIntegrityCheck(const Table *pTab, const Table *pObjTab);
  __attribute__((noinline)) int columnIsGoodIndexCandidate(const Table *pTab, int iCol);

#ifdef __cplusplus
}
#endif
