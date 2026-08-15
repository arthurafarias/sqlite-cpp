
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "_TypeIndex.h"

typedef struct Wal Wal;

struct Wal {
  sqlite3_vfs *pVfs;
  sqlite3_file *pDbFd;
  sqlite3_file *pWalFd;
  u32 iCallback;
  i64 mxWalSize;
  int nWiData;
  int szFirstBlock;
  volatile u32 **apWiData;
  u32 szPage;
  i16 readLock;
  u8 syncFlags;
  u8 exclusiveMode;
  u8 writeLock;
  u8 ckptLock;
  u8 readOnly;
  u8 truncateOnCommit;
  u8 syncHeader;
  u8 padToSectorBoundary;
  u8 bShmUnreliable;
  WalIndexHdr hdr;
  u32 minFrame;
  u32 iReCksum;
  const char *zWalName;
  u32 nCkpt;
};

#ifdef __cplusplus
}
#endif

