
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite3_int64.h"
#include "sqlite3_uint64.h"
#include "sqlite3_value.h"

  typedef struct sqlite3_context sqlite3_context;
  typedef struct sqlite3 sqlite3;

  struct sqlite3_context {
  Mem *pOut;
  FuncDef *pFunc;
  Mem *pMem;
  Vdbe *pVdbe;
  int iOp;
  int isError;
  u8 enc;
  u8 skipFlag;
  u16 argc;
  sqlite3_value *argv[];
};

  int sqlite3_aggregate_count(sqlite3_context *);
  int sqlite3_result_zeroblob64(sqlite3_context *, sqlite3_uint64 n);
  sqlite3 *sqlite3_context_db_handle(sqlite3_context *);
  typedef void (*sqlite3_destructor_type)(void *);
  void *sqlite3_aggregate_context(sqlite3_context *, int nBytes);
  void *sqlite3_get_auxdata(sqlite3_context *, int N);
  void *sqlite3_user_data(sqlite3_context *);
  void sqlite3_result_blob(sqlite3_context *, const void *, int, void (*)(void *));
  void sqlite3_result_blob64(sqlite3_context *, const void *, sqlite3_uint64, void (*)(void *));
  void sqlite3_result_double(sqlite3_context *, double);
  void sqlite3_result_error_code(sqlite3_context *, int);
  void sqlite3_result_error_nomem(sqlite3_context *);
  void sqlite3_result_error_toobig(sqlite3_context *);
  void sqlite3_result_error(sqlite3_context *, const char *, int);
  void sqlite3_result_error16(sqlite3_context *, const void *, int);
  void sqlite3_result_int(sqlite3_context *, int);
  void sqlite3_result_int64(sqlite3_context *, sqlite3_int64);
  void sqlite3_result_null(sqlite3_context *);
  void sqlite3_result_pointer(sqlite3_context *, void *, const char *, void (*)(void *));
  void sqlite3_result_subtype(sqlite3_context *, unsigned int);
  void sqlite3_result_text(sqlite3_context *, const char *, int, void (*)(void *));
  void sqlite3_result_text16(sqlite3_context *, const void *, int, void (*)(void *));
  void sqlite3_result_text16be(sqlite3_context *, const void *, int, void (*)(void *));
  void sqlite3_result_text16le(sqlite3_context *, const void *, int, void (*)(void *));
  void sqlite3_result_text64(sqlite3_context *, const char *z, sqlite3_uint64 n, void (*)(void *), unsigned char encoding);
  void sqlite3_result_value(sqlite3_context *, sqlite3_value *);
  void sqlite3_result_zeroblob(sqlite3_context *, int n);
  void sqlite3_set_auxdata(sqlite3_context *, int N, void *, void (*)(void *));
  int sqlite3_vtab_nochange(sqlite3_context *);
static int sqlite3NotPureFunc(sqlite3_context *);

#ifdef __cplusplus
}
#endif
