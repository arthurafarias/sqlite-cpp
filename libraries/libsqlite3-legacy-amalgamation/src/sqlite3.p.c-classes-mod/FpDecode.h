
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct FpDecode FpDecode;

struct FpDecode {
  int n;
  int iDP;
  char *z;
  char zBuf[20 + 1];
  char sign;
  char isSpecial;
};

#ifdef __cplusplus
}
#endif

