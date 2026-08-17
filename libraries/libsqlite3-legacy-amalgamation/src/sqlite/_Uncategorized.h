#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/LogEst.h"
#include "sqlite/NanInfName.h"
#include "sqlite/PragmaName.h"
#include "sqlite/et_info.h"
#include "sqlite/i16.h"
#include "sqlite/i64.h"
#include "sqlite/sqlite3_int64.h"
#include "sqlite/sqlite3_uint64.h"
#include "sqlite/tRowcnt.h"
#include "sqlite/u16.h"
#include "sqlite/u32.h"
#include "sqlite/u64.h"
#include "sqlite/u8.h"
#include "sqlite/unix_syscall.h"
#include <stdarg.h>
#include <sys/types.h>
#include <time.h>
typedef struct PgHdr DbPage;
typedef struct Expr Expr;
typedef struct Index Index;
typedef struct JsonString JsonString;
typedef struct sqlite3_value Mem;
typedef struct Parse Parse;
typedef struct SrcItem SrcItem;
typedef struct Table Table;
typedef struct UnixUnusedFd UnixUnusedFd;
typedef struct WhereClause WhereClause;
typedef struct compareInfo compareInfo;
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_context sqlite3_context;
typedef struct sqlite3_io_methods sqlite3_io_methods;
typedef struct sqlite3_mutex sqlite3_mutex;
typedef struct sqlite3_pcache sqlite3_pcache;
typedef struct unixFile unixFile;
typedef struct sqlite3_index_constraint_usage sqlite3_index_constraint_usage;
/* Functions with no clear owning container (free helpers, glibc-shadowed
   symbols, etc.) collected here for manual triage. */

typedef struct aXformType_t aXformType_t;

struct aXformType_t {
  u8 nName;
  char zName[7];
  float rLimit;
  float rXform;
};

extern const aXformType_t aXformType[6];
extern const char row_numberName[11];
extern const char dense_rankName[11];
extern const char rankName[5];
extern const char percent_rankName[13];
extern const char cume_distName[10];
extern const char ntileName[6];
extern const char last_valueName[11];
extern const char nth_valueName[10];
extern const char first_valueName[12];
extern const char leadName[5];
extern const char lagName[4];
extern const char *const sqlite3azCompileOpt[40];
extern const unsigned char *sqlite3aLTb;
extern const unsigned char *sqlite3aEQb;
extern const unsigned char *sqlite3aGTb;
extern const char statMutex[10];
extern const char aDigits[33];
extern const char aPrefix[7];
extern const et_info fmtinfo[23];
extern pid_t randomnessPid;
extern struct unix_syscall aSyscall[29];
extern const char *const azExplainColNames8[12];
extern const u16 azExplainColNames16data[60];
extern const u8 iExplainColNames16[12];
extern const char *azTempDirs[6];
extern const char zMagicHeader[16];
extern const char *const pragCName[57];
extern const PragmaName aPragmaName[66];
extern const struct ExprList_item zeroItem;
extern const unsigned short yyRuleInfoLhs[412];
extern const signed char yyRuleInfoNRhs[412];
extern const unsigned char aiClass[256];
extern const char zKWText[666];
extern const unsigned char aKWHash[127];
extern const unsigned char aKWNext[148];
extern const unsigned char aKWLen[148];
extern const unsigned short aKWOffset[148];
extern const unsigned char aKWCode[148];
extern const unsigned short yy_action[2379];
extern const unsigned short yy_lookahead[2566];
extern const unsigned short yy_shift_ofst[600];
extern const short yy_reduce_ofst[424];
extern const unsigned short yy_default[600];
extern const unsigned short yyFallback[187];
extern const char *const jsonbType[17];
extern const char jsonIsSpace[256];
extern const char jsonSpaces[5];
extern const char jsonIsOk[256];
extern const int aHardLimit[13];
extern const struct NanInfName aNanInfName[5];
extern const unsigned char aJournalMagic[8];
extern const char hexdigits[16];

extern const sqlite3_io_methods *(*const nolockIoFinder)(const char *, unixFile *);
extern const sqlite3_io_methods *(*const dotlockIoFinder)(const char *, unixFile *);

int getDigits(const char *zDate, const char *zFormat, ...);
int validJulianDay(sqlite3_int64 iJD);
int osLocaltime(time_t *t, struct tm *pTm);
int noopMutexInit(void);
int noopMutexEnd(void);
sqlite3_mutex *noopMutexAlloc(int id);
int pthreadMutexInit(void);
int pthreadMutexEnd(void);
sqlite3_mutex *pthreadMutexAlloc(int iType);
void mallocWithAlarm(int n, void **pp);
void renderLogMsg(int iErrCode, const char *zFormat, va_list ap);
void chacha_block(u32 *out, const u32 *in);
u64 powerOfTen(int p, u32 *pLo);
int pwr10to2(int p);
int pwr2to10(int p);
int countLeadingZeros(u64 m);
int compare2pow63(const char *zNum, int incr);
int __attribute__((noinline)) putVarint64(unsigned char *p, u64 v);
void logBadConnection(const char *zType);
unsigned int strHash(const char *z);
int posixOpen(const char *zFile, int flags, int mode);
int openDirectory(const char *, int *);
int unixGetpagesize(void);
int robustFchown(int fd, uid_t uid, gid_t gid);
int robust_open(const char *z, int f, mode_t m);
void unixEnterMutex(void);
void unixLeaveMutex(void);
int robust_ftruncate(int h, sqlite3_int64 sz);
int sqliteErrorFromPosixError(int posixError, int sqliteIOErr);
int unixLogErrorAtLine(int errcode, const char *zFunc, const char *zPath, int iLine);
int seekAndWriteFd(int fd, i64 iOff, const void *pBuf, int nBuf, int *piErrno);
int full_fsync(int fd, int fullSync, int dataOnly);
int unixGetTempname(int nBuf, char *zBuf);
void unixTempFileInit(void);
const char *unixTempFileDir(void);
UnixUnusedFd *findReusableFd(const char *zPath, int flags);
int getFileMode(const char *zFile, mode_t *pMode, uid_t *pUid, gid_t *pGid);
int findCreateFileMode(const char *zPath, int flags, mode_t *pMode, uid_t *pUid, gid_t *pGid);
void *pcache1Alloc(int nByte);
void pcache1Free(void *p);
int pcache1Init(void *NotUsed);
void pcache1Shutdown(void *NotUsed);
sqlite3_pcache *pcache1Create(int szPage, int szExtra, int bPurgeable);
void freeSuperJournal(char *zSuper);
int copyPayload(void *pPayload, void *pBuf, int nByte, int eOp, DbPage *pDbPage);
int isFatalError(int rc);
void serialGet(const unsigned char *buf, u32 serial_type, Mem *pMem);
int serialGet7(const unsigned char *buf, Mem *pMem);
int isAllZero(const char *z, int n);
int invokeValueDestructor(const void *p, void (*xDel)(void *), sqlite3_context *pCtx);
i64 findNextHostParameter(const char *zSql, i64 *pnToken);
__attribute__((noinline)) int isValidSchemaTableName(const char *zTab, Table *pTab, const char *zDb);
int getConstraintToken(const u8 *z, int *piToken);
int getWhitespace(const u8 *z);
int getConstraint(const u8 *z);
void decodeIntArray(char *zIntArray, int nOut, tRowcnt *aOut, LogEst *aLog, Index *pIndex);
int analysisLoader(void *pData, int argc, char **argv, char **NotUsed);
i64 identLength(const char *z);
void identPut(char *z, int *pIdx, char *zSignedIdent);
int hasColumn(const i16 *aiCol, int nCol, int x);
int collationMatch(const char *zColl, Index *pIndex);
int patternCompare(const u8 *zPattern, const u8 *zString, const struct compareInfo *pInfo, u32 matchOther);
int isNHex(const char *z, int N, u32 *pVal);
int strContainsChar(const u8 *zStr, int nStr, u32 ch);
double xCeil(double x);
double xFloor(double x);
double degToRad(double x);
double radToDeg(double x);
int percentIsInfinity(double r);
int percentSameValue(double a, double b);
void percentSort(double *a, unsigned int n);
u8 getSafetyLevel(const char *z, int omitFull, u8 dflt);
int getLockingMode(const char *z);
int getAutoVacuum(const char *z);
int getTempStore(const char *z);
const char *actionName(u8 action);
const PragmaName *pragmaLocate(const char *zName);
int inAnyUsingClause(const char *zName, SrcItem *pBase, int N);
int allowedOp(int op);
u16 operatorMask(int op);
LogEst estLog(LogEst N);
int whereUsablePartialIndex(int iTab, u8 jointype, WhereClause *pWC, Expr *pWhere);
int allConstraintsUsed(sqlite3_index_constraint_usage *aUsage, int nCons);
void *parserStackRealloc(void *pOld, sqlite3_uint64 newSize, Parse *pParse);
void parserStackFree(void *pOld, Parse *pParse);
unsigned short int yy_find_shift_action(unsigned short int iLookAhead, unsigned short int stateno);
unsigned short int yy_find_reduce_action(unsigned short int stateno, unsigned short int iLookAhead);
i64 keywordCode(const char *z, i64 n, int *pType);
int getToken(const unsigned char **pz);
int analyzeWindowKeyword(const unsigned char *z);
int analyzeOverKeyword(const unsigned char *z, int lastToken);
int analyzeFilterKeyword(const unsigned char *z, int lastToken);
int binCollFunc(void *NotUsed, int nKey1, const void *pKey1, int nKey2, const void *pKey2);
int rtrimCollFunc(void *pUser, int nKey1, const void *pKey1, int nKey2, const void *pKey2);
int nocaseCollatingFunc(void *NotUsed, int nKey1, const void *pKey1, int nKey2, const void *pKey2);
int sqliteDefaultBusyCallback(void *ptr, int count);
const char *uriParameter(const char *zFilename, const char *zParam);
int openDatabase(const char *zFilename, sqlite3 **ppDb, unsigned int flags, const char *zVfs);
const char *databaseName(const char *zName);
char *appendText(char *p, const char *z);
u32 jsonUnescapeOneChar(const char *, u32, u32 *);
void jsonPrintf(int N, JsonString *p, const char *zFormat, ...);
u8 jsonHexToInt(int h);
u32 jsonHexToInt4(const char *z);
int jsonIs2Hex(const char *z);
int jsonIs4Hex(const char *z);
int json5Whitespace(const char *zIn);
int jsonIs4HexB(const char *z, int *pOp);
int jsonBlobOverwrite(u8 *aOut, const u8 *aIns, u32 nIns, u32 d);
u32 jsonBytesToBypass(const char *z, u32 n);
__attribute__((noinline)) int jsonLabelCompareEscaped(const char *zLeft, u32 nLeft, int rawLeft, const char *zRight, u32 nRight, int rawRight);
int jsonLabelCompare(const char *zLeft, u32 nLeft, int rawLeft, const char *zRight, u32 nRight, int rawRight);
int jsonAllAlphanum(const char *z, int n);




#ifdef __cplusplus
}
#endif
