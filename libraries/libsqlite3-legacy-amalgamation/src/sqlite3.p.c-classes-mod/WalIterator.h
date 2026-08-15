
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct WalIterator WalIterator;
struct WalIterator {
  u32 iPrior;
  int nSegment;
  struct WalSegment {
    int iNext;
    ht_slot *aIndex;
    u32 *aPgno;
    int nEntry;
    int iZero;
  } aSegment[];
};

#ifdef __cplusplus
}
#endif

