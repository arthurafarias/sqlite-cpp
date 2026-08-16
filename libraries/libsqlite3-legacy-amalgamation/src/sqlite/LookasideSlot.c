#include "sqlite/_All.h"

u32 countLookasideSlots(LookasideSlot *p) {
  u32 cnt = 0;
  while (p) {
    p = p->pNext;
    cnt++;
  }
  return cnt;
}
