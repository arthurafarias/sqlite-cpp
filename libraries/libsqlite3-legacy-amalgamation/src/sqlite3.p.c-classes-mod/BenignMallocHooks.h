
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct BenignMallocHooks BenignMallocHooks;
typedef struct BenignMallocHooks BenignMallocHooks;
static struct BenignMallocHooks {
  void (*xBenignBegin)(void);
  void (*xBenignEnd)(void);
} sqlite3Hooks = {0, 0};

#ifdef __cplusplus
}
#endif

