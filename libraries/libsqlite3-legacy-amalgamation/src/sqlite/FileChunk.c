#include "sqlite/_All.h"

void memjrnlFreeChunks(FileChunk *pFirst) {
  FileChunk *pIter;
  FileChunk *pNext;
  for (pIter = pFirst; pIter; pIter = pNext) {
    pNext = pIter->pNext;
    sqlite3_free(pIter);
  }
}
