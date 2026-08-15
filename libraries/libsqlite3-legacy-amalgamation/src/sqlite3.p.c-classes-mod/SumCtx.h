
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct SumCtx SumCtx;
struct SumCtx {
  double rSum;
  double rErr;
  i64 iSum;
  i64 cnt;
  u8 approx;
  u8 ovrfl;
};

#ifdef __cplusplus
}
#endif

