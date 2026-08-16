
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/_TypeIndex.h"

  typedef struct WindowCsrAndReg WindowCsrAndReg;

  struct WindowCsrAndReg {
    int csr;
    int reg;
  };

#ifdef __cplusplus
}
#endif
