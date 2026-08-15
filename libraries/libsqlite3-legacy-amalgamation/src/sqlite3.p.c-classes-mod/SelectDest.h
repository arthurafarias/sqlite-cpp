
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct SelectDest SelectDest;


struct SelectDest {
  u8 eDest;
  int iSDParm;
  int iSDParm2;
  int iSdst;
  int nSdst;
  char *zAffSdst;
  ExprList *pOrderBy;
};

#ifdef __cplusplus
}
#endif

