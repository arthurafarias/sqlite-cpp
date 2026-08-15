
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct SQLiteThread SQLiteThread;

struct SQLiteThread {
  pthread_t tid;
  int done;
  void *pOut;
  void *(*xTask)(void *);
  void *pIn;
};

#ifdef __cplusplus
}
#endif

