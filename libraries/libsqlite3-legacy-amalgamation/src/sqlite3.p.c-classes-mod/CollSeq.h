
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct CollSeq CollSeq;

struct CollSeq {
  char *zName;
  u8 enc;
  void *pUser;
  int (*xCmp)(void *, int, const void *, int, const void *);
  void (*xDel)(void *);
};

#ifdef __cplusplus
}
#endif

