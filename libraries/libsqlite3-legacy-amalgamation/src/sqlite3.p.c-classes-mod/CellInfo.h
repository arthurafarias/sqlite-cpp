
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct CellInfo CellInfo;


struct CellInfo {
  i64 nKey;
  u8 *pPayload;
  u32 nPayload;
  u16 nLocal;
  u16 nSize;
};

#ifdef __cplusplus
}
#endif

