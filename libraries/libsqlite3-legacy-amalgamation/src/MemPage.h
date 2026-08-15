
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct MemPage MemPage;


struct MemPage {
  u8 isInit;
  u8 intKey;
  u8 intKeyLeaf;
  Pgno pgno;

  u8 leaf;
  u8 hdrOffset;
  u8 childPtrSize;
  u8 max1bytePayload;
  u8 nOverflow;
  u16 maxLocal;
  u16 minLocal;
  u16 cellOffset;
  int nFree;
  u16 nCell;
  u16 maskPage;
  u16 aiOvfl[4];

  u8 *apOvfl[4];
  BtShared *pBt;
  u8 *aData;
  u8 *aDataEnd;

  u8 *aCellIdx;
  u8 *aDataOfst;
  DbPage *pDbPage;
  u16 (*xCellSize)(MemPage *, u8 *);
  void (*xParseCell)(MemPage *, u8 *, CellInfo *);
};

#ifdef __cplusplus
}
#endif

