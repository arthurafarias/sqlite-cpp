typedef struct {
  sqlite3 *db;
  char **pzErrMsg;
  int iDb;
  int rc;
  u32 mInitFlags;
  u32 nInitRow;
  Pgno mxPage;
} InitData;