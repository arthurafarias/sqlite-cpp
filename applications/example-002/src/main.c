#include <stdio.h>

#include "sqlite/sqlite3_state.h"
#include "sqlite/sqlite3_stmt.h"

int main(void) {
  sqlite3_state *db;
  sqlite3_stmt *stmt;

  sqlite3_state_open("expenses.db", &db);

  if (db == NULL) {
    printf("Failed to open DB\n");
    return 1;
  }

  printf("Performing query...\n");
  sqlite3_state_prepare_v2(db, "select * from expenses", -1, &stmt, NULL);

  printf("Got results:\n");

  // TODO: From the sqlite3.h library convert all #defines into enumerations en put them into the src folder from libsqlite3-legacy-amalgamation following the project structure pattern
  while (sqlite3_state_step(stmt) != SQLITE_DONE) {
    int i;
    int num_cols = sqlite3_state_column_count(stmt);

    for (i = 0; i < num_cols; i++) {
      switch (sqlite3_state_column_type(stmt, i)) {
      case (SQLITE3_TEXT):
        printf("%s, ", sqlite3_state_column_text(stmt, i));
        break;
      case (SQLITE_INTEGER):
        printf("%d, ", sqlite3_state_column_int(stmt, i));
        break;
      case (SQLITE_FLOAT):
        printf("%g, ", sqlite3_state_column_double(stmt, i));
        break;
      default:
        break;
      }
    }
    printf("\n");
  }

  sqlite3_state_finalize(stmt);

  sqlite3_state_close(db);

  getc(stdin);
  return 0;
}