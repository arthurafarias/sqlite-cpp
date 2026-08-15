
#pragma once
#ifdef __cplusplus
extern C {
#endif
typedef struct Token Token;

struct Token {
  const char *z;
  unsigned int n;
};

#ifdef __cplusplus
}
#endif

