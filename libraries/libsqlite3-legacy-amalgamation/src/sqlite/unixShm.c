#define _GNU_SOURCE 1

#include "sqlite/unixShm.h"

#include "sqlite/sqlite3_syscall_ptr.h"
#include "sqlite/unix_syscall.h"
int unixShmRegionPerMap(void) {
  int shmsz = 32 * 1024;
  int pgsz = ((int (*)(void))aSyscall[25].pCurrent)();

  if (pgsz < shmsz)
    return 1;
  return pgsz / shmsz;
}
