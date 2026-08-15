
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct BtShared BtShared;


struct BtShared {
  Pager *pPager;
  sqlite3 *db;
  BtCursor *pCursor;
  MemPage *pPage1;
  u8 openFlags;

  u8 autoVacuum;
  u8 incrVacuum;
  u8 bDoTruncate;

  u8 inTransaction;
  u8 max1bytePayload;
  u8 nReserveWanted;
  u16 btsFlags;
  u16 maxLocal;
  u16 minLocal;
  u16 maxLeaf;
  u16 minLeaf;
  u32 pageSize;
  u32 usableSize;
  int nTransaction;
  u32 nPage;
  void *pSchema;
  void (*xFreeSchema)(void *);
  sqlite3_mutex *mutex;
  Bitvec *pHasContent;

  int nRef;
  BtShared *pNext;
  BtLock *pLock;
  Btree *pWriter;

  u8 *pTmpSpace;
  int nPreformatSize;
};

#ifdef __cplusplus
}
#endif

