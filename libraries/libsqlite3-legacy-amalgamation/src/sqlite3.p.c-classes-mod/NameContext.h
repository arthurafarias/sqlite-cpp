
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct NameContext NameContext;


struct NameContext {
  Parse *pParse;
  SrcList *pSrcList;
  union {
    ExprList *pEList;
    AggInfo *pAggInfo;
    Upsert *pUpsert;
    int iBaseReg;
  } uNC;
  NameContext *pNext;
  int nRef;
  int nNcErr;
  int ncFlags;
  u32 nNestedSelect;
  Select *pWinSelect;
};

#ifdef __cplusplus
}
#endif

