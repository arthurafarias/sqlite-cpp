
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Bitvec Bitvec;
struct Bitvec {
  u32 iSize;
  u32 nSet;

  u32 iDivisor;

  union {
    u8 aBitmap[((((512 - (3 * sizeof(u32))) / sizeof(Bitvec *)) * sizeof(Bitvec *)) / sizeof(u8))];
    u32 aHash[((((512 - (3 * sizeof(u32))) / sizeof(Bitvec *)) * sizeof(Bitvec *)) / sizeof(u32))];
    Bitvec *apSub[((u32)((((512 - (3 * sizeof(u32))) / sizeof(Bitvec *)) * sizeof(Bitvec *)) / sizeof(Bitvec *)))];
  } u;

};

#ifdef __cplusplus
}
#endif

