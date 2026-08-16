
#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/sqlite3_str.h"

typedef struct sqlite3_str StrAccum;

void sqlite3QuoteValue(StrAccum *, sqlite3_value *, int);
void sqlite3StrAccumInit(StrAccum *, sqlite3 *, char *, int, int);
int sqlite3StrAccumEnlarge(StrAccum *, i64);
int sqlite3StrAccumEnlargeIfNeeded(StrAccum *, i64);
char *sqlite3StrAccumFinish(StrAccum *);
void sqlite3StrAccumSetError(StrAccum *, u8);
void __attribute__((noinline)) enlargeAndAppend(StrAccum *p, const char *z, int N);
__attribute__((noinline)) char *strAccumFinishRealloc(StrAccum *p);
void explainAppendTerm(StrAccum *pStr, Index *pIdx, int nTerm, int iTerm, int bAnd, const char *zOp);
void explainIndexRange(StrAccum *pStr, WhereLoop *pLoop);

#ifdef __cplusplus
}
#endif