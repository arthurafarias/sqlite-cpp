#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i16.h"
#include "i32.h"
#include "i64.h"
#include "i8.h"
#include "u16.h"
#include "u32.h"
#include "u64.h"
#include "u8.h"

#include "sqlite_int64.h"
#include "sqlite_uint64.h"

#include "sqlite3_filename.h"
#include "sqlite3_int64.h"
#include "sqlite3_rtree_dbl.h"
#include "sqlite3_uint64.h"

#include "AuxData.h"
#include "BtCursor.h"
#include "Btree.h"
#include "CollSeq.h"
#include "Column.h"
#include "Cte.h"
#include "Expr.h"
#include "ExprList.h"
#include "FuncDef.h"
#include "FuncDestructor.h"
#include "IdList.h"
#include "Index.h"
#include "KeyInfo.h"
#include "Module.h"
#include "OnOrUsing.h"
#include "Op.h"
#include "Parse.h"
#include "Pgno.h"
#include "RowSet.h"
#include "Schema.h"
#include "Select.h"
#include "SrcItem.h"
#include "SrcList.h"
#include "Subquery.h"
#include "Table.h"
#include "Token.h"
#include "Trigger.h"
#include "TriggerStep.h"
#include "UnpackedRecord.h"
#include "Upsert.h"
#include "VList.h"
#include "VTable.h"
#include "Vdbe.h"
#include "VdbeCursor.h"
#include "VdbeSorter.h"
#include "Window.h"
#include "With.h"
#include "sqlite3_backup.h"
#include "sqlite3_blob.h"
#include "sqlite3_context.h"
#include "sqlite3_module.h"
#include "sqlite3_mutex.h"
#include "sqlite3_rtree_geometry.h"
#include "sqlite3_rtree_query_info.h"
#include "sqlite3_snapshot.h"
#include "sqlite3_stmt.h"
#include "sqlite3_str.h"
#include "sqlite3_value.h"

typedef struct sqlite3 sqlite3;

const char *sqlite3_db_name(sqlite3 *db, int N);
const char *sqlite3_errmsg(sqlite3 *);
const void *sqlite3_errmsg16(sqlite3 *);
int sqlite3_autovacuum_pages(sqlite3 *db, unsigned int (*)(void *, const char *, unsigned int, unsigned int, unsigned int), void *, void (*)(void *));
int sqlite3_blob_open(sqlite3 *, const char *zDb, const char *zTable, const char *zColumn, sqlite3_int64 iRow, int flags, sqlite3_blob **ppBlob);
int sqlite3_busy_handler(sqlite3 *, int (*)(void *, int), void *);
int sqlite3_busy_timeout(sqlite3 *, int ms);
int sqlite3_changes(sqlite3 *);
int sqlite3_close_v2(sqlite3 *);
int sqlite3_close(sqlite3 *);
int sqlite3_collation_needed(sqlite3 *, void *, void (*)(void *, sqlite3 *, int eTextRep, const char *));
int sqlite3_collation_needed16(sqlite3 *, void *, void (*)(void *, sqlite3 *, int eTextRep, const void *));
int sqlite3_create_collation_v2(sqlite3 *, const char *zName, int eTextRep, void *pArg, int (*xCompare)(void *, int, const void *, int, const void *), void (*xDestroy)(void *));
int sqlite3_create_collation(sqlite3 *, const char *zName, int eTextRep, void *pArg, int (*xCompare)(void *, int, const void *, int, const void *));
int sqlite3_create_collation16(sqlite3 *, const void *zName, int eTextRep, void *pArg, int (*xCompare)(void *, int, const void *, int, const void *));
int sqlite3_create_function_v2(sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void *pApp, void (*xFunc)(sqlite3_context *, int, sqlite3_value **), void (*xStep)(sqlite3_context *, int, sqlite3_value **), void (*xFinal)(sqlite3_context *), void (*xDestroy)(void *));
int sqlite3_create_function(sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void *pApp, void (*xFunc)(sqlite3_context *, int, sqlite3_value **), void (*xStep)(sqlite3_context *, int, sqlite3_value **), void (*xFinal)(sqlite3_context *));
int sqlite3_create_function16(sqlite3 *db, const void *zFunctionName, int nArg, int eTextRep, void *pApp, void (*xFunc)(sqlite3_context *, int, sqlite3_value **), void (*xStep)(sqlite3_context *, int, sqlite3_value **), void (*xFinal)(sqlite3_context *));
int sqlite3_create_module_v2(sqlite3 *db, const char *zName, const sqlite3_module *p, void *pClientData, void (*xDestroy)(void *));
int sqlite3_create_module(sqlite3 *db, const char *zName, const sqlite3_module *p, void *pClientData);
int sqlite3_create_window_function(sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void *pApp, void (*xStep)(sqlite3_context *, int, sqlite3_value **), void (*xFinal)(sqlite3_context *), void (*xValue)(sqlite3_context *), void (*xInverse)(sqlite3_context *, int, sqlite3_value **), void (*xDestroy)(void *));
int sqlite3_db_cacheflush(sqlite3 *);
int sqlite3_db_config(sqlite3 *, int op, ...);
int sqlite3_db_readonly(sqlite3 *db, const char *zDbName);
int sqlite3_db_release_memory(sqlite3 *);
int sqlite3_db_status(sqlite3 *, int op, int *pCur, int *pHiwtr, int resetFlg);
int sqlite3_db_status64(sqlite3 *, int, sqlite3_int64 *, sqlite3_int64 *, int);
int sqlite3_declare_vtab(sqlite3 *, const char *zSQL);
int sqlite3_deserialize(sqlite3 *db, const char *zSchema, unsigned char *pData, sqlite3_int64 szDb, sqlite3_int64 szBuf, unsigned mFlags);
int sqlite3_drop_modules(sqlite3 *db, const char **azKeep);
int sqlite3_enable_load_extension(sqlite3 *db, int onoff);
int sqlite3_errcode(sqlite3 *db);
int sqlite3_error_offset(sqlite3 *db);
int sqlite3_exec(sqlite3 *, const char *sql, int (*callback)(void *, int, char **, char **), void *, char **errmsg);
int sqlite3_extended_errcode(sqlite3 *db);
int sqlite3_extended_result_codes(sqlite3 *, int onoff);
int sqlite3_file_control(sqlite3 *, const char *zDbName, int op, void *);
int sqlite3_get_autocommit(sqlite3 *);
int sqlite3_get_table(sqlite3 *db, const char *zSql, char ***pazResult, int *pnRow, int *pnColumn, char **pzErrmsg);
int sqlite3_is_interrupted(sqlite3 *);
int sqlite3_limit(sqlite3 *, int id, int newVal);
int sqlite3_load_extension(sqlite3 *db, const char *zFile, const char *zProc, char **pzErrMsg);
int sqlite3_open_v2(const char *filename, sqlite3 **ppDb, int flags, const char *zVfs);
int sqlite3_open(const char *filename, sqlite3 **ppDb);
int sqlite3_open16(const void *filename, sqlite3 **ppDb);
int sqlite3_overload_function(sqlite3 *, const char *zFuncName, int nArg);
int sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
int sqlite3_prepare_v3(sqlite3 *db, const char *zSql, int nByte, unsigned int prepFlags, sqlite3_stmt **ppStmt, const char **pzTail);
int sqlite3_prepare(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
int sqlite3_prepare16_v2(sqlite3 *db, const void *zSql, int nByte, sqlite3_stmt **ppStmt, const void **pzTail);
int sqlite3_prepare16_v3(sqlite3 *db, const void *zSql, int nByte, unsigned int prepFlags, sqlite3_stmt **ppStmt, const void **pzTail);
int sqlite3_prepare16(sqlite3 *db, const void *zSql, int nByte, sqlite3_stmt **ppStmt, const void **pzTail);
int sqlite3_rtree_geometry_callback(sqlite3 *db, const char *zGeom, int (*xGeom)(sqlite3_rtree_geometry *, int, sqlite3_rtree_dbl *, int *), void *pContext);
int sqlite3_rtree_query_callback(sqlite3 *db, const char *zQueryFunc, int (*xQueryFunc)(sqlite3_rtree_query_info *), void *pContext, void (*xDestructor)(void *));
int sqlite3_set_authorizer(sqlite3 *, int (*xAuth)(void *, int, const char *, const char *, const char *, const char *), void *pUserData);
int sqlite3_set_clientdata(sqlite3 *, const char *, void *, void (*)(void *));
int sqlite3_set_errmsg(sqlite3 *db, int errcode, const char *zErrMsg);
int sqlite3_setlk_timeout(sqlite3 *, int ms, int flags);
int sqlite3_snapshot_get(sqlite3 *db, const char *zSchema, sqlite3_snapshot **ppSnapshot);
int sqlite3_snapshot_open(sqlite3 *db, const char *zSchema, sqlite3_snapshot *pSnapshot);
int sqlite3_snapshot_recover(sqlite3 *db, const char *zDb);
int sqlite3_system_errno(sqlite3 *);
int sqlite3_table_column_metadata(sqlite3 *db, const char *zDbName, const char *zTableName, const char *zColumnName, char const **pzDataType, char const **pzCollSeq, int *pNotNull, int *pPrimaryKey, int *pAutoinc);
int sqlite3_total_changes(sqlite3 *);
int sqlite3_trace_v2(sqlite3 *, unsigned uMask, int (*xCallback)(unsigned, void *, void *, void *), void *pCtx);
int sqlite3_txn_state(sqlite3 *, const char *zSchema);
int sqlite3_unlock_notify(sqlite3 *pBlocked, void (*xNotify)(void **apArg, int nArg), void *pNotifyArg);
int sqlite3_vtab_config(sqlite3 *, int op, ...);
int sqlite3_vtab_on_conflict(sqlite3 *);
int sqlite3_wal_autocheckpoint(sqlite3 *db, int N);
int sqlite3_wal_checkpoint_v2(sqlite3 *db, const char *zDb, int eMode, int *pnLog, int *pnCkpt);
int sqlite3_wal_checkpoint(sqlite3 *db, const char *zDb);
sqlite3_backup *sqlite3_backup_init(sqlite3 *pDest, const char *zDestName, sqlite3 *pSource, const char *zSourceName);
sqlite3_filename sqlite3_db_filename(sqlite3 *db, const char *zDbName);
sqlite3_int64 sqlite3_changes64(sqlite3 *);
sqlite3_int64 sqlite3_last_insert_rowid(sqlite3 *);
sqlite3_int64 sqlite3_total_changes64(sqlite3 *);
sqlite3_mutex *sqlite3_db_mutex(sqlite3 *);
sqlite3_stmt *sqlite3_next_stmt(sqlite3 *pDb, sqlite3_stmt *pStmt);
sqlite3_str *sqlite3_str_new(sqlite3 *);
static Btree *sqlite3DbNameToBtree(sqlite3 *, const char *);
static char *sqlite3DbSpanDup(sqlite3 *, const char *, const char *);
static char *sqlite3DbStrDup(sqlite3 *, const char *);
static char *sqlite3DbStrNDup(sqlite3 *, const char *, u64);
static char *sqlite3MPrintf(sqlite3 *, const char *, ...);
static char *sqlite3NameFromToken(sqlite3 *, const Token *);
static char *sqlite3TableAffinityStr(sqlite3 *, const Table *);
static char *sqlite3Utf16to8(sqlite3 *, const void *, int, u8);
static char *sqlite3VdbeDisplayP4(sqlite3 *, Op *);
static char *sqlite3VMPrintf(sqlite3 *, const char *, va_list);

static CollSeq *sqlite3FindCollSeq(sqlite3 *, u8 enc, const char *, int);
static const char *sqlite3IndexAffinityStr(sqlite3 *, Index *);

static Expr *sqlite3CreateColumnExpr(sqlite3 *, SrcList *, int, int);
static Expr *sqlite3Expr(sqlite3 *, int, const char *);
static Expr *sqlite3ExprAlloc(sqlite3 *, int, const Token *, int);
static Expr *sqlite3ExprDup(sqlite3 *, const Expr *, int);
static Expr *sqlite3ExprInt32(sqlite3 *, int);

static ExprList *sqlite3ExprListDup(sqlite3 *, const ExprList *, int);
static FuncDef *sqlite3FindFunction(sqlite3 *, const char *, int, u8, u8);
static FuncDef *sqlite3VtabOverloadFunction(sqlite3 *, FuncDef *, int nArg, Expr *);
static IdList *sqlite3IdListDup(sqlite3 *, const IdList *);
static Index *sqlite3AllocateIndexObject(sqlite3 *, int, int, char **);
static Index *sqlite3FindIndex(sqlite3 *, const char *, const char *);
static int sqlite3AnalysisLoad(sqlite3 *, int iDB);
static int sqlite3ApiExit(sqlite3 *db, int);
static int sqlite3BtreeCount(sqlite3 *, BtCursor *, i64 *);
static int sqlite3BtreeIntegrityCheck(sqlite3 *db, Btree *p, Pgno *aRoot, sqlite3_value *aCnt, int nRoot, int mxErr, int *pnErr, char **pzOut);
static int sqlite3Checkpoint(sqlite3 *, int, int, int *, int *);
static int sqlite3CreateFunc(sqlite3 *, const char *, int, int, void *, void (*)(sqlite3_context *, int, sqlite3_value **), void (*)(sqlite3_context *, int, sqlite3_value **), void (*)(sqlite3_context *), void (*)(sqlite3_context *), void (*)(sqlite3_context *, int, sqlite3_value **), FuncDestructor *pDestructor);
static int sqlite3DbIsNamed(sqlite3 *db, int iDb, const char *zName);
static int sqlite3DbMallocSize(sqlite3 *, const void *);
static int sqlite3ErrorToParser(sqlite3 *, int);
static int sqlite3FindDb(sqlite3 *, Token *);
static int sqlite3FindDbName(sqlite3 *, const char *);
static int sqlite3Init(sqlite3 *, char **);
static int sqlite3InitOne(sqlite3 *, int, char **, u32);
static int sqlite3IsLikeFunction(sqlite3 *, Expr *, int *, char *);
static int sqlite3IsShadowTableOf(sqlite3 *, Table *, const char *);
static int sqlite3LookasideUsed(sqlite3 *, int *);
static int sqlite3ReadOnlyShadowTables(sqlite3 *db);
static int sqlite3SafetyCheckOk(sqlite3 *);
static int sqlite3SafetyCheckSickOrOk(sqlite3 *);
static int sqlite3SchemaToIndex(sqlite3 *db, Schema *);
static int sqlite3ShadowTableName(sqlite3 *db, const char *zName);
static int sqlite3ValueFromExpr(sqlite3 *, const Expr *, u8, u8, sqlite3_value **);
static int sqlite3VdbeIdxKeyCompare(sqlite3 *, VdbeCursor *, UnpackedRecord *, int *);
static int sqlite3VdbeIdxRowid(sqlite3 *, BtCursor *, i64 *);
static int sqlite3VdbeSorterInit(sqlite3 *, int, VdbeCursor *);
static int sqlite3VdbeSorterNext(sqlite3 *, const VdbeCursor *);
static int sqlite3VtabBegin(sqlite3 *, VTable *);
static int sqlite3VtabCallCreate(sqlite3 *, int, const char *, char **);
static int sqlite3VtabCallDestroy(sqlite3 *, int, const char *);
static int sqlite3VtabCommit(sqlite3 *db);
static int sqlite3VtabRollback(sqlite3 *db);
static int sqlite3VtabSavepoint(sqlite3 *, int, int);
static int sqlite3VtabSync(sqlite3 *db, Vdbe *);
static int sqlite3WritableSchema(sqlite3 *);
static KeyInfo *sqlite3KeyInfoAlloc(sqlite3 *, int, int);
static Module *sqlite3JsonVtabRegister(sqlite3 *, const char *);
static Module *sqlite3PragmaVtabRegister(sqlite3 *, const char *zName);
static Module *sqlite3VtabCreateModule(sqlite3 *, const char *, const sqlite3_module *, void *, void (*)(void *));
static RowSet *sqlite3RowSetInit(sqlite3 *);
static Schema *sqlite3SchemaGet(sqlite3 *, Btree *);
static Select *sqlite3SelectDup(sqlite3 *, const Select *, int);
static Select *sqlite3SubqueryDetach(sqlite3 *, SrcItem *);
static sqlite3_value *sqlite3ValueNew(sqlite3 *);
static SrcList *sqlite3SrcListDup(sqlite3 *, const SrcList *, int);
static Table *sqlite3FindTable(sqlite3 *, const char *, const char *);
static TriggerStep *sqlite3TriggerSelectStep(sqlite3 *, Select *, const char *, const char *);
static Upsert *sqlite3UpsertDup(sqlite3 *, Upsert *);
static Upsert *sqlite3UpsertNew(sqlite3 *, ExprList *, Expr *, ExprList *, Expr *, Upsert *);
static VList *sqlite3VListAdd(sqlite3 *, VList *, const char *, int, int);
static void *sqlite3ArrayAllocate(sqlite3 *, void *, int, int *, int *);
static void *sqlite3DbMallocRaw(sqlite3 *, u64);
static void *sqlite3DbMallocRawNN(sqlite3 *, u64);
static void *sqlite3DbMallocZero(sqlite3 *, u64);
static void *sqlite3DbRealloc(sqlite3 *, void *, u64);
static void *sqlite3DbReallocOrFree(sqlite3 *, void *, u64);
static void *sqlite3HexToBlob(sqlite3 *, const char *z, int n);
static void *sqlite3OomFault(sqlite3 *);
static void *sqlite3ParserAddCleanup(Parse *, void (*)(sqlite3 *, void *), void *);
static void sqlite3AutoLoadExtensions(sqlite3 *);
static void sqlite3BtreeEnterAll(sqlite3 *);
static void sqlite3BtreeLeaveAll(sqlite3 *);
static void sqlite3ClearOnOrUsing(sqlite3 *, OnOrUsing *);
static void sqlite3CloseExtensions(sqlite3 *);
static void sqlite3CloseSavepoints(sqlite3 *);
static void sqlite3CollapseDatabaseArray(sqlite3 *);
static void sqlite3ColumnSetColl(sqlite3 *, Column *, const char *zColl);
static void sqlite3CommitInternalChanges(sqlite3 *);
static void sqlite3CteDelete(sqlite3 *, Cte *);
static void sqlite3DbFree(sqlite3 *, void *);
static void sqlite3DbFreeNN(sqlite3 *, void *);
static void sqlite3DbNNFreeNN(sqlite3 *, void *);
static void sqlite3DeleteColumnNames(sqlite3 *, Table *);
static void sqlite3DeleteIndexSamples(sqlite3 *, Index *);
static void sqlite3DeleteTable(sqlite3 *, Table *);
static void sqlite3DeleteTableGeneric(sqlite3 *, void *);
static void sqlite3DeleteTrigger(sqlite3 *, Trigger *);
static void sqlite3DeleteTriggerStep(sqlite3 *, TriggerStep *);
static void sqlite3Error(sqlite3 *, int);
static void sqlite3ErrorClear(sqlite3 *);
static void sqlite3ErrorWithMsg(sqlite3 *, int, const char *, ...);
static void sqlite3ExpirePreparedStatements(sqlite3 *, int);
static void sqlite3ExprAttachSubtrees(sqlite3 *, Expr *, Expr *, Expr *);
static void sqlite3ExprDelete(sqlite3 *, Expr *);
static void sqlite3ExprDeleteGeneric(sqlite3 *, void *);
static void sqlite3ExprListDelete(sqlite3 *, ExprList *);
static void sqlite3ExprListDeleteGeneric(sqlite3 *, void *);
static void sqlite3FkClearTriggerCache(sqlite3 *, int);
static void sqlite3FkDelete(sqlite3 *, Table *);
static void sqlite3FreeIndex(sqlite3 *, Index *);
static void sqlite3IdListDelete(sqlite3 *, IdList *);
static void sqlite3LeaveMutexAndCloseZombie(sqlite3 *);
static void sqlite3MarkAllShadowTablesOf(sqlite3 *, Table *);
static void sqlite3OomClear(sqlite3 *);
static void sqlite3RecordErrorByteOffset(sqlite3 *, const char *);
static void sqlite3RecordErrorOffsetOfExpr(sqlite3 *, const Expr *);
static void sqlite3RegisterLikeFunctions(sqlite3 *, int);
static void sqlite3RegisterPerConnectionBuiltinFunctions(sqlite3 *);
static void sqlite3ResetAllSchemasOfConnection(sqlite3 *);
static void sqlite3ResetOneSchema(sqlite3 *, int);
static void sqlite3RollbackAll(sqlite3 *, int);
static void sqlite3RootPageMoved(sqlite3 *, int, Pgno, Pgno);
static void sqlite3SelectDelete(sqlite3 *, Select *);
static void sqlite3SelectDeleteGeneric(sqlite3 *, void *);
static void sqlite3SetTextEncoding(sqlite3 *db, u8);
static void sqlite3SrcListDelete(sqlite3 *, SrcList *);
static void sqlite3SubqueryDelete(sqlite3 *, Subquery *);
static void sqlite3SystemError(sqlite3 *, int);
static void sqlite3UnlinkAndDeleteIndex(sqlite3 *, int, const char *);
static void sqlite3UnlinkAndDeleteTable(sqlite3 *, int, const char *);
static void sqlite3UnlinkAndDeleteTrigger(sqlite3 *, int, const char *);
static void sqlite3UpsertDelete(sqlite3 *, Upsert *);
static void sqlite3VdbeDeleteAuxData(sqlite3 *, AuxData **, int, int);
static void sqlite3VdbeSetChanges(sqlite3 *, i64);
static void sqlite3VdbeSorterClose(sqlite3 *, VdbeCursor *);
static void sqlite3VdbeSorterReset(sqlite3 *, VdbeSorter *);
static void sqlite3VtabClear(sqlite3 *db, Table *);
static void sqlite3VtabDisconnect(sqlite3 *db, Table *p);
static void sqlite3VtabEponymousTableClear(sqlite3 *, Module *);
static void sqlite3VtabModuleUnref(sqlite3 *, Module *);
static void sqlite3VtabUnlockList(sqlite3 *);
static void sqlite3WindowDelete(sqlite3 *, Window *);
static void sqlite3WindowListDelete(sqlite3 *db, Window *p);
static void sqlite3WithDelete(sqlite3 *, With *);
static void sqlite3WithDeleteGeneric(sqlite3 *, void *);
static VTable *sqlite3GetVTable(sqlite3 *, Table *);
static Window *sqlite3WindowDup(sqlite3 *db, Expr *pOwner, Window *p);
static Window *sqlite3WindowListDup(sqlite3 *db, Window *p);
static With *sqlite3WithDup(sqlite3 *db, With *p);
unsigned char *sqlite3_serialize(sqlite3 *db, const char *zSchema, sqlite3_int64 *piSize, unsigned int mFlags);
void *sqlite3_commit_hook(sqlite3 *, int (*)(void *), void *);
void *sqlite3_get_clientdata(sqlite3 *, const char *);
void *sqlite3_profile(sqlite3 *, void (*xProfile)(void *, const char *, sqlite3_uint64), void *);
void *sqlite3_rollback_hook(sqlite3 *, void (*)(void *), void *);
void *sqlite3_trace(sqlite3 *, void (*xTrace)(void *, const char *), void *);
void *sqlite3_update_hook(sqlite3 *, void (*)(void *, int, char const *, char const *, sqlite3_int64), void *);
void *sqlite3_wal_hook(sqlite3 *, int (*)(void *, sqlite3 *, const char *, int), void *);
void sqlite3_interrupt(sqlite3 *);
void sqlite3_progress_handler(sqlite3 *, int, int (*)(void *), void *);
void sqlite3_set_last_insert_rowid(sqlite3 *, sqlite3_int64);

#ifdef __cplusplus
}
#endif