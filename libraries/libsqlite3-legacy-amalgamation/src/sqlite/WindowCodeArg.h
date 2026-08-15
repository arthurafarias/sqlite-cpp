
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/WindowCsrAndReg.h"
#include "sqlite/_TypeIndex.h"

  typedef struct WindowCodeArg WindowCodeArg;

  struct WindowCodeArg {
    Parse *pParse;
    Window *pMWin;
    Vdbe *pVdbe;
    int addrGosub;
    int regGosub;
    int regArg;
    int eDelete;
    int regRowid;

    WindowCsrAndReg start;
    WindowCsrAndReg current;
    WindowCsrAndReg end;
  };

#ifdef __cplusplus
}
#endif
