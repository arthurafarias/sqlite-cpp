
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/Bitvec.h"
#include "sqlite/_TypeIndex.h"
#include "sqlite/sqlite3_libversion.h"
#include "sqlite/sqlite3_libversion_number.h"
#include "sqlite/sqlite3_sourceid.h"

#include "sqlite/u8.h"

  typedef struct Pager Pager;
  typedef struct sqlite3 sqlite3;
  typedef struct BtCursor BtCursor;
  typedef struct MemPage MemPage;

  typedef struct BtShared BtShared;
  typedef struct sqlite3_mutex sqlite3_mutex;
  typedef struct Bitvec Bitvec;

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
