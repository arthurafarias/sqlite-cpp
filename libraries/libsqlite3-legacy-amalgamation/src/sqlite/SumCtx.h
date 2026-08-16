
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct SumCtx SumCtx;
  struct SumCtx {
    double rSum;
    double rErr;
    i64 iSum;
    i64 cnt;
    u8 approx;
    u8 ovrfl;
  };

  void kahanBabuskaNeumaierStep(volatile SumCtx * pSum, volatile double r);
  void kahanBabuskaNeumaierStepInt64(volatile SumCtx * pSum, i64 iVal);
  void kahanBabuskaNeumaierInit(volatile SumCtx * p, i64 iVal);

#ifdef __cplusplus
}
#endif
