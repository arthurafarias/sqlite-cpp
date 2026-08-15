
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite3_int64.h"
#include "_TypeIndex.h"

  typedef struct DateTime DateTime;
  struct DateTime {
    sqlite3_int64 iJD;
    int Y, M, D;
    int h, m;
    int tz;
    double s;
    char validJD;
    char validYMD;
    char validHMS;
    char nFloor;
    unsigned rawS : 1;
    unsigned isError : 1;
    unsigned useSubsec : 1;
    unsigned isUtc : 1;
    unsigned isLocal : 1;
  };

#ifdef __cplusplus
}
#endif
