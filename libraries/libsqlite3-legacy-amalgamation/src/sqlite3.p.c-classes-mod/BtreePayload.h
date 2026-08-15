
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct BtreePayload BtreePayload;

struct BtreePayload {
  const void *pKey;
  sqlite3_int64 nKey;
  const void *pData;
  sqlite3_value *aMem;
  u16 nMem;
  int nData;
  int nZero;
};

#ifdef __cplusplus
}
#endif

