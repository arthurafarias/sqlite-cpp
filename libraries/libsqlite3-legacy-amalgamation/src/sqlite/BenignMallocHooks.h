
#pragma once
#ifdef __cplusplus
extern C {
#endif

  typedef struct BenignMallocHooks BenignMallocHooks;
  
  struct BenignMallocHooks {
    void (*xBenignBegin)(void);
    void (*xBenignEnd)(void);
  };

  extern BenignMallocHooks sqlite3Hooks;

#ifdef __cplusplus
}
#endif
