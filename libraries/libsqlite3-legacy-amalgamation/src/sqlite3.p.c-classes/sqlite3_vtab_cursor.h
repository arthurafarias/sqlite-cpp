
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct sqlite3_vtab_cursor sqlite3_vtab_cursor;
typedef struct sqlite3_vtab sqlite3_vtab;

struct sqlite3_vtab_cursor {
  sqlite3_vtab *pVtab;
};

#ifdef __cplusplus
}
#endif

