
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct FuncDestructor FuncDestructor;

struct FuncDestructor {
  int nRef;
  void (*xDestroy)(void *);
  void *pUserData;
};

#ifdef __cplusplus
}
#endif

