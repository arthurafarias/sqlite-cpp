
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct unixShm unixShm;
struct unixShm {
  unixShmNode *pShmNode;
  unixShm *pNext;
  u8 hasMutex;
  u8 id;
  u16 sharedMask;
  u16 exclMask;
};
#ifdef __cplusplus
}
#endif

