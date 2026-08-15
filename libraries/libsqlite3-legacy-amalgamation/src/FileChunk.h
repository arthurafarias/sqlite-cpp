
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "u8.h"

  typedef struct FileChunk FileChunk;
  struct FileChunk {
    FileChunk *pNext;
    u8 zChunk[8];
  };

#ifdef __cplusplus
}
#endif
