
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct UnpackedRecord UnpackedRecord;


struct UnpackedRecord {
  KeyInfo *pKeyInfo;
  Mem *aMem;
  union {
    char *z;
    i64 i;
  } u;
  int n;
  u16 nField;
  i8 default_rc;
  u8 errCode;
  i8 r1;
  i8 r2;
  u8 eqSeen;
};

#ifdef __cplusplus
}
#endif

