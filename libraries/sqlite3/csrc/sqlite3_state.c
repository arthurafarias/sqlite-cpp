#include "sqlite/sqlite3_state.h"

int sqlite3_state_open(const char *filename, sqlite3_state **ppDb) {
  return sqlite3_open(filename, ppDb);
}

int sqlite3_state_close(sqlite3_state *db) { return sqlite3_close(db); }

int sqlite3_state_prepare_v2(sqlite3_state *db, const char *zSql, int nByte,
                              sqlite3_stmt **ppStmt, const char **pzTail) {
  return sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
}

int sqlite3_state_step(sqlite3_stmt *stmt) { return sqlite3_step(stmt); }

int sqlite3_state_finalize(sqlite3_stmt *stmt) { return sqlite3_finalize(stmt); }

int sqlite3_state_column_count(sqlite3_stmt *stmt) {
  return sqlite3_column_count(stmt);
}

int sqlite3_state_column_type(sqlite3_stmt *stmt, int iCol) {
  return sqlite3_column_type(stmt, iCol);
}

const unsigned char *sqlite3_state_column_text(sqlite3_stmt *stmt, int iCol) {
  return sqlite3_column_text(stmt, iCol);
}

int sqlite3_state_column_int(sqlite3_stmt *stmt, int iCol) {
  return sqlite3_column_int(stmt, iCol);
}

double sqlite3_state_column_double(sqlite3_stmt *stmt, int iCol) {
  return sqlite3_column_double(stmt, iCol);
}
