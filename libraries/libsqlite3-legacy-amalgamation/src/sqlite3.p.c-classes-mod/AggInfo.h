
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct AggInfo AggInfo;


struct AggInfo {
  u8 directMode;

  u8 useSortingIdx;

  u32 nSortingColumn;
  int sortingIdx;
  int sortingIdxPTab;
  int iFirstReg;
  ExprList *pGroupBy;
  struct AggInfo_col {
    Table *pTab;
    Expr *pCExpr;
    int iTable;
    int iColumn;
    int iSorterColumn;
  } *aCol;
  int nColumn;
  int nAccumulator;

  struct AggInfo_func {
    Expr *pFExpr;
    FuncDef *pFunc;
    int iDistinct;
    int iDistAddr;
    int iOBTab;
    u8 bOBPayload;
    u8 bOBUnique;
    u8 bUseSubtype;
  } *aFunc;
  int nFunc;
  u32 selId;
};

#ifdef __cplusplus
}
#endif

