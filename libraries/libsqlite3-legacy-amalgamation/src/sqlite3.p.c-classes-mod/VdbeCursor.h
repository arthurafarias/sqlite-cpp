
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct VdbeCursor VdbeCursor;
struct VdbeCursor {
  u8 eCurType;
  i8 iDb;
  u8 nullRow;
  u8 deferredMoveto;
  u8 isTable;

  Bool isEphemeral : 1;
  Bool useRandomRowid : 1;
  Bool isOrdered : 1;
  Bool noReuse : 1;
  Bool colCache : 1;
  u16 seekHit;
  union {
    Btree *pBtx;
    u32 *aAltMap;
  } ub;
  i64 seqCount;

  u32 cacheStatus;
  int seekResult;
  VdbeCursor *pAltCursor;
  union {
    BtCursor *pCursor;
    sqlite3_vtab_cursor *pVCur;
    VdbeSorter *pSorter;
  } uc;
  KeyInfo *pKeyInfo;
  u32 iHdrOffset;
  Pgno pgnoRoot;
  i16 nField;
  u16 nHdrParsed;
  i64 movetoTarget;
  u32 *aOffset;
  const u8 *aRow;
  u32 payloadSize;
  u32 szRow;

  VdbeTxtBlbCache *pCache;

  u32 aType[];
};

#ifdef __cplusplus
}
#endif

