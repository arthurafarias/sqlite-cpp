
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct BtCursor BtCursor;


struct BtCursor {
  u8 eState;
  u8 curFlags;
  u8 curPagerFlags;
  u8 hints;
  int skipNext;

  Btree *pBtree;
  Pgno *aOverflow;
  void *pKey;

  BtShared *pBt;
  BtCursor *pNext;
  CellInfo info;
  i64 nKey;
  Pgno pgnoRoot;
  i8 iPage;
  u8 curIntKey;
  u16 ix;
  u16 aiIdx[20 - 1];
  struct KeyInfo *pKeyInfo;
  MemPage *pPage;
  MemPage *apPage[20 - 1];
};

#ifdef __cplusplus
}
#endif


