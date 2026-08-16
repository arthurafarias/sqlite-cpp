
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/sqlite3_syscall_ptr.h"

typedef struct unix_syscall unix_syscall;

struct unix_syscall {
  const char *zName;
  sqlite3_syscall_ptr pCurrent;
  sqlite3_syscall_ptr pDefault;
};

#ifdef __cplusplus
}
#endif
