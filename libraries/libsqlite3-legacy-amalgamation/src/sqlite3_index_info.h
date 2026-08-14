
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite3_int64.h"
#include "sqlite3_uint64.h"

  typedef struct sqlite3_index_info sqlite3_index_info;

  struct sqlite3_index_info {

    int nConstraint;
    struct sqlite3_index_constraint {
      int iColumn;
      unsigned char op;
      unsigned char usable;
      int iTermOffset;
    } *aConstraint;
    int nOrderBy;
    struct sqlite3_index_orderby {
      int iColumn;
      unsigned char desc;
    } *aOrderBy;

    struct sqlite3_index_constraint_usage {
      int argvIndex;
      unsigned char omit;
    } *aConstraintUsage;
    int idxNum;
    char *idxStr;
    int needToFreeIdxStr;
    int orderByConsumed;
    double estimatedCost;

    sqlite3_int64 estimatedRows;

    int idxFlags;

    sqlite3_uint64 colUsed;
  };

#ifdef __cplusplus
}
#endif
