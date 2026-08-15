
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct AuthContext AuthContext;

struct AuthContext {
  const char *zAuthContext;
  Parse *pParse;
};

#ifdef __cplusplus
}
#endif

