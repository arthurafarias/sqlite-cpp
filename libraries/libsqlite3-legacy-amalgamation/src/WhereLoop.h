
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct WhereLoop WhereLoop;

struct WhereLoop {
  Bitmask prereq;
  Bitmask maskSelf;

  u8 iTab;
  u8 iSortIdx;
  LogEst rSetup;
  LogEst rRun;
  LogEst nOut;
  union {
    struct {
      u16 nEq;
      u16 nBtm;
      u16 nTop;
      u16 nDistinctCol;
      Index *pIndex;
      ExprList *pOrderBy;
    } btree;
    struct {
      int idxNum;
      u32 needFree : 1;
      u32 bOmitOffset : 1;
      u32 bIdxNumHex : 1;
      i8 isOrdered;
      u16 omitMask;
      char *idxStr;
      u32 mHandleIn;
    } vtab;
  } u;
  u32 wsFlags;
  u16 nLTerm;
  u16 nSkip;

  u16 nLSlot;

  WhereTerm **aLTerm;
  WhereLoop *pNextLoop;
  WhereTerm *aLTermSpace[3];
};
#ifdef __cplusplus
}
#endif

