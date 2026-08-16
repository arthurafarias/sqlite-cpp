#include "sqlite/_All.h"

int unixShmRegionPerMap(void) {
  int shmsz = 32 * 1024;
  int pgsz = ((int (*)(void))aSyscall[25].pCurrent)();

  if (pgsz < shmsz)
    return 1;
  return pgsz / shmsz;
}
