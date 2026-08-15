
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct ParseCleanup ParseCleanup;

struct ParseCleanup {
  ParseCleanup *pNext;
  void *pPtr;
  void (*xCleanup)(sqlite3 *, void *);
};

#ifdef __cplusplus
}
#endif

