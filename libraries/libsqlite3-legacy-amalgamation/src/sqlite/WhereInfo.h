
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

#include "sqlite/BitMask.h"
#include "sqlite/WhereClause.h"
#include "sqlite/WhereLevel.h"
#include "sqlite/WhereMaskSet.h"

typedef struct WhereInfo WhereInfo;

struct WhereInfo {
  Parse *pParse;
  SrcList *pTabList;
  ExprList *pOrderBy;
  ExprList *pResultSet;

  Select *pSelect;
  int aiCurOnePass[2];
  int iContinue;
  int iBreak;
  int savedNQueryLoop;
  u16 wctrlFlags;
  LogEst iLimit;
  u8 nLevel;
  i8 nOBSat;
  u8 eOnePass;
  u8 eDistinct;
  unsigned bDeferredSeek : 1;
  unsigned untestedTerms : 1;
  unsigned bOrderedInnerLoop : 1;
  unsigned sorted : 1;
  unsigned bStarDone : 1;
  unsigned bStarUsed : 1;
  LogEst nRowOut;

  int iTop;
  int iEndWhere;
  WhereLoop *pLoops;
  WhereMemBlock *pMemToFree;
  Bitmask revMask;
  WhereClause sWC;
  WhereMaskSet sMaskSet;
  WhereLevel a[];
};

#ifdef __cplusplus
}
#endif

