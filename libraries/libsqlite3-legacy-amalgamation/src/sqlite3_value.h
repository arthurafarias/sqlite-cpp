
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite3_int64.h"

typedef struct sqlite3_value sqlite3_value;

const void *sqlite3_value_blob(sqlite3_value *);
double sqlite3_value_double(sqlite3_value *);
int sqlite3_value_int(sqlite3_value *);
sqlite3_int64 sqlite3_value_int64(sqlite3_value *);
void *sqlite3_value_pointer(sqlite3_value *, const char *);
const unsigned char *sqlite3_value_text(sqlite3_value *);
const void *sqlite3_value_text16(sqlite3_value *);
const void *sqlite3_value_text16le(sqlite3_value *);
const void *sqlite3_value_text16be(sqlite3_value *);
int sqlite3_value_bytes(sqlite3_value *);
int sqlite3_value_bytes16(sqlite3_value *);
int sqlite3_value_type(sqlite3_value *);
int sqlite3_value_numeric_type(sqlite3_value *);
int sqlite3_value_nochange(sqlite3_value *);
int sqlite3_value_frombind(sqlite3_value *);
int sqlite3_value_encoding(sqlite3_value *);
unsigned int sqlite3_value_subtype(sqlite3_value *);
sqlite3_value *sqlite3_value_dup(const sqlite3_value *);
void sqlite3_value_free(sqlite3_value *);

#ifdef __cplusplus
}
#endif

