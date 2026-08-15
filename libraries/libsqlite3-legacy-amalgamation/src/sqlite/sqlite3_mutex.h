
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include <pthread.h>

#include "sqlite/_TypeIndex.h"

  typedef struct sqlite3_mutex sqlite3_mutex;

  struct sqlite3_mutex {
    pthread_mutex_t mutex;
  };

  sqlite3_mutex *sqlite3_mutex_alloc(int);
  void sqlite3_mutex_free(sqlite3_mutex *);
  void sqlite3_mutex_enter(sqlite3_mutex *);
  int sqlite3_mutex_try(sqlite3_mutex *);
  void sqlite3_mutex_leave(sqlite3_mutex *);
  int sqlite3_mutex_held(sqlite3_mutex *);

  int sqlite3_mutex_held(sqlite3_mutex *);
  int sqlite3_mutex_notheld(sqlite3_mutex *);

#ifdef __cplusplus
}
#endif
