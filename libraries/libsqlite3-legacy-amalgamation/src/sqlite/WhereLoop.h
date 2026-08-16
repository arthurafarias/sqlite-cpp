
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/BitMask.h"
#include "sqlite/_TypeIndex.h"

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

  int whereLoopIsOneRow(WhereLoop * pLoop);
  void whereLoopInit(WhereLoop * p);
  int whereLoopCheaperProperSubset(const WhereLoop *pX, const WhereLoop *pY);
  void whereLoopAdjustCost(const WhereLoop *p, WhereLoop *pTemplate);
  WhereLoop **whereLoopFindLesser(WhereLoop * *ppPrev, const WhereLoop *pTemplate);
  __attribute__((noinline)) int whereLoopIsNoBetter(const WhereLoop *pCandidate, const WhereLoop *pBaseline);

#ifdef __cplusplus
}
#endif
