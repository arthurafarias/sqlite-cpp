#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/SqliteFundamentalDatatype.h"
#include "sqlite/SqliteResultCode.h"
#include "sqlite/sqlite3.h"
#include "sqlite/sqlite3_stmt.h"

/* SRS 005: the public sqlite3-cpp entry point. `struct sqlite3` (defined
 * across the split subsystem libraries this umbrella links) is renamed at
 * this boundary only -- sqlite3_state is a type alias for it, not a
 * second, differently-laid-out struct, so every sqlite3_state_* function
 * below is a thin forwarding call into the real sqlite3_* API. */
typedef sqlite3 sqlite3_state;

int sqlite3_state_open(const char *filename, sqlite3_state **ppDb);
int sqlite3_state_close(sqlite3_state *db);
int sqlite3_state_prepare_v2(sqlite3_state *db, const char *zSql, int nByte,
                              sqlite3_stmt **ppStmt, const char **pzTail);
int sqlite3_state_step(sqlite3_stmt *stmt);
int sqlite3_state_finalize(sqlite3_stmt *stmt);
int sqlite3_state_column_count(sqlite3_stmt *stmt);
int sqlite3_state_column_type(sqlite3_stmt *stmt, int iCol);
const unsigned char *sqlite3_state_column_text(sqlite3_stmt *stmt, int iCol);
int sqlite3_state_column_int(sqlite3_stmt *stmt, int iCol);
double sqlite3_state_column_double(sqlite3_stmt *stmt, int iCol);

#ifdef __cplusplus
}
#endif
