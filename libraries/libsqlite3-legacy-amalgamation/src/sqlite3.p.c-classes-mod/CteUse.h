
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct CteUse CteUse;


struct CteUse {
  int nUse;
  int addrM9e;
  int regRtn;
  int iCur;
  LogEst nRowEst;
  u8 eM10d;
};

#ifdef __cplusplus
}
#endif

