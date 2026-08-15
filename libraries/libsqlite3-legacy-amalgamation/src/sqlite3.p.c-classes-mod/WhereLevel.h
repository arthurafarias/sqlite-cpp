
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct WhereLevel WhereLevel;
struct WhereLevel {
  int iLeftJoin;
  int iTabCur;
  int iIdxCur;
  int addrBrk;
  int addrHalt;
  int addrNxt;
  int addrSkip;
  int addrCont;
  int addrFirst;
  int addrBody;
  int regBignull;
  int addrBignull;

  u32 iLikeRepCntr;
  int addrLikeRep;

  int regFilter;
  WhereRightJoin *pRJ;
  u8 iFrom;
  u8 op, p3, p5;
  int p1, p2;
  union {
    struct {
      int nIn;
      struct InLoop {
        int iCur;
        int addrInTop;
        int iBase;
        int nPrefix;
        u8 eEndLoopOp;
      } *aInLoop;
    } in;
    Index *pCoveringIdx;
  } u;
  struct WhereLoop *pWLoop;
  Bitmask notReady;
};


#ifdef __cplusplus
}
#endif

