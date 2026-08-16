#pragma once

#ifdef __cplusplus
#endif

#include "sqlite/i16.h"
#include "sqlite/i32.h"
#include "sqlite/i64.h"
#include "sqlite/i8.h"

#include "sqlite/u16.h"
#include "sqlite/u32.h"
#include "sqlite/u64.h"
#include "sqlite/u8.h"

#include "sqlite/sqlite3_int64.h"
#include "sqlite/sqlite3_uint64.h"

typedef struct sqlite3_api_routines sqlite3_api_routines;
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
typedef long double long_double_t;
typedef long int Lmid_t;
typedef short int LogEst;
typedef sqlite3_int64 sqlite3StatValueType;
typedef struct AggInfo AggInfo;
typedef struct analysisInfo analysisInfo;
typedef struct AuthContext AuthContext;
typedef struct AutoincInfo AutoincInfo;
typedef struct AuxData AuxData;
typedef struct BenignMallocHooks BenignMallocHooks;
typedef struct BitMask BitMask;
typedef struct Bitvec Bitvec;
typedef struct BtCursor BtCursor;
typedef struct BtLock BtLock;
typedef struct Btree Btree;
typedef struct BtreePayload BtreePayload;
typedef struct BtShared BtShared;
typedef struct BusyHandler BusyHandler;
typedef struct CellArray CellArray;
typedef struct CellInfo CellInfo;
typedef struct CheckOnCtx CheckOnCtx;
typedef struct CollSeq CollSeq;
typedef struct Column Column;
typedef struct CountCtx CountCtx;
typedef struct CoveringIndexCheck CoveringIndexCheck;
typedef struct Cte Cte;
typedef struct CteUse CteUse;
typedef struct DateTime DateTime;
typedef struct Db Db;
typedef struct DbClientData DbClientData;
typedef struct DbFixer DbFixer;
typedef struct DblquoteStr DblquoteStr;
typedef struct DbPath DbPath;
typedef struct DistinctCtx DistinctCtx;
typedef struct EdupBuf EdupBuf;
typedef struct Expr Expr;
typedef struct ExprList ExprList;
typedef struct FileChunk FileChunk;
typedef struct FilePoint FilePoint;
typedef struct FKey FKey;
typedef struct FpDecode FpDecode;
typedef struct fts5_api fts5_api;
typedef struct fts5_tokenizer fts5_tokenizer;
typedef struct fts5_tokenizer_v2 fts5_tokenizer_v2;
typedef struct Fts5Context Fts5Context;
typedef struct Fts5ExtensionApi Fts5ExtensionApi;
typedef struct Fts5PhraseIter Fts5PhraseIter;
typedef struct Fts5Tokenizer Fts5Tokenizer;
typedef struct FuncDef FuncDef;
typedef struct FuncDefHash FuncDefHash;
typedef struct FuncDestructor FuncDestructor;
typedef struct FrameBound FrameBound;
typedef struct Hash Hash;
typedef struct HashElem HashElem;
typedef struct HiddenIndexInfo HiddenIndexInfo;
typedef struct IdList IdList;
typedef struct Incrblob Incrblob;
typedef struct IncrMerger IncrMerger;
typedef struct Index Index;
typedef struct IndexedExpr IndexedExpr;
typedef struct IndexIterator IndexIterator;
typedef struct IndexListTerm IndexListTerm;
typedef struct IndexSample IndexSample;
typedef struct InitData InitData;
typedef struct IntegrityCk IntegrityCk;
typedef struct JsonCache JsonCache;
typedef struct JsonEachConnection JsonEachConnection;
typedef struct JsonEachCursor JsonEachCursor;
typedef struct JsonParent JsonParent;
typedef struct JsonParse JsonParse;
typedef struct JsonPretty JsonPretty;
typedef struct JsonString JsonString;
typedef struct KeyClass KeyClass;
typedef struct KeyInfo KeyInfo;
typedef struct Lookaside Lookaside;
typedef struct LookasideSlot LookasideSlot;
typedef struct sqlite3_value Mem;
typedef struct MemFile MemFile;
typedef struct MemFS MemFS;
typedef struct MemJournal MemJournal;
typedef struct MemPage MemPage;
typedef struct MemStore MemStore;
typedef struct MergeEngine MergeEngine;
typedef struct Module Module;
typedef struct NameContext NameContext;
typedef struct OnOrUsing OnOrUsing;
typedef struct Pager Pager;
typedef struct PagerSavepoint PagerSavepoint;
typedef struct Parse Parse;
typedef struct ParseCleanup ParseCleanup;
typedef struct PCache PCache;
typedef struct PCache1 PCache1;
typedef struct PgFreeslot PgFreeslot;
typedef struct PgHdr PgHdr;
typedef struct PgHdr DbPage;
typedef struct PgHdr1 PgHdr1;
typedef struct sqlite3_pcache sqlite3_pcache;
typedef struct VdbeOp Op;
typedef struct PragmaName PragmaName;
typedef struct PGroup PGroup;
typedef struct PmaReader PmaReader;
typedef struct PmaWriter PmaWriter;
typedef struct PragmaVtab PragmaVtab;
typedef struct PragmaVtabCursor PragmaVtabCursor;
typedef struct PreUpdate PreUpdate;
typedef struct PrintfArguments PrintfArguments;
typedef struct RCStr RCStr;
typedef struct RenameCtx RenameCtx;
typedef struct RenameToken RenameToken;
typedef struct Returning Returning;
typedef struct RowLoadInfo RowLoadInfo;
typedef struct RowSet RowSet;
typedef struct Savepoint Savepoint;
typedef struct ScanStatus ScanStatus;
typedef struct Schema Schema;
typedef struct Select Select;
typedef struct SelectDest SelectDest;
typedef struct SortCtx SortCtx;
typedef struct SorterFile SorterFile;
typedef struct SorterList SorterList;
typedef struct SorterRecord SorterRecord;
typedef struct SortSubtask SortSubtask;
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_api_routines sqlite3_api_routines;
typedef struct sqlite3_backup sqlite3_backup;
typedef struct sqlite3_blob sqlite3_blob;
typedef struct sqlite3_context sqlite3_context;
typedef struct sqlite3_hard_heap sqlite3_hard_heap;
typedef struct sqlite3_index_info sqlite3_index_info;
typedef struct sqlite3_io_methods sqlite3_io_methods;
typedef struct sqlite3_mem_methods sqlite3_mem_methods;
typedef struct sqlite3_module sqlite3_module;
typedef struct sqlite3_mutex sqlite3_mutex;
typedef struct sqlite3_mutex_methods sqlite3_mutex_methods;
typedef struct sqlite3_pcache_methods sqlite3_pcache_methods;
typedef struct sqlite3_pcache_methods2 sqlite3_pcache_methods2;
typedef struct sqlite3_pcache_page sqlite3_pcache_page;
typedef struct sqlite3_rtree_geometry sqlite3_rtree_geometry;
typedef struct sqlite3_rtree_query_info sqlite3_rtree_query_info;
typedef struct sqlite3_snapshot sqlite3_snapshot;
typedef struct sqlite3_soft_heap sqlite3_soft_heap;
typedef struct Vdbe sqlite3_stmt;
typedef struct sqlite3_str sqlite3_str;
typedef struct sqlite3_value sqlite3_value;
typedef struct sqlite3_vfs MemVfs;
typedef struct sqlite3_vfs sqlite3_vfs;
typedef struct sqlite3_vtab sqlite3_vtab;
typedef struct sqlite3_vtab_cursor sqlite3_vtab_cursor;
typedef struct sqlite3_file sqlite3_file;
typedef struct sqlite3AutoExtList sqlite3AutoExtList;
typedef struct Sqlite3Config Sqlite3Config;
typedef struct sqlite3StatType sqlite3StatType;
typedef struct SQLiteThread SQLiteThread;
typedef struct SrcItem SrcItem;
typedef struct SrcList SrcList;
typedef struct StatAccum StatAccum;
typedef struct StatSample StatSample;
typedef struct SubProgram SubProgram;
typedef struct Subquery Subquery;
typedef struct SubrtnSig SubrtnSig;
typedef struct SumCtx SumCtx;
typedef struct Table Table;
typedef struct TableLock TableLock;
typedef struct Token Token;
typedef struct TreeView TreeView;
typedef struct Trigger Trigger;
typedef struct TriggerPrg TriggerPrg;
typedef struct TriggerStep TriggerStep;
typedef struct TrigEvent TrigEvent;
typedef struct unixFile unixFile;
typedef struct unixFileId unixFileId;
typedef struct unixInodeInfo unixInodeInfo;
typedef struct unixShm unixShm;
typedef struct unixShmNode unixShmNode;
typedef struct UnixUnusedFd UnixUnusedFd;
typedef struct UnpackedRecord UnpackedRecord;
typedef struct Upsert Upsert;
typedef struct ValueList ValueList;
typedef struct Vdbe Vdbe;
typedef struct VdbeCursor VdbeCursor;
typedef struct VdbeFrame VdbeFrame;
typedef struct VdbeOp VdbeOp;
typedef struct VdbeOpList VdbeOpList;
typedef struct VdbeSorter VdbeSorter;
typedef struct VdbeTxtBlbCache VdbeTxtBlbCache;
typedef struct VtabCtx VtabCtx;
typedef struct VTable VTable;
typedef struct Wal Wal;
typedef struct WalCkptInfo WalCkptInfo;
typedef struct WalHashLoc WalHashLoc;
typedef struct WalIndexHdr WalIndexHdr;
typedef struct WalIterator WalIterator;
typedef struct Walker Walker;
typedef struct WhereAndInfo WhereAndInfo;
typedef struct WhereClause WhereClause;
typedef struct WhereConst WhereConst;
typedef struct WhereInfo WhereInfo;
typedef struct WhereLevel WhereLevel;
typedef struct WhereLoop WhereLoop;
typedef struct WhereLoopBuilder WhereLoopBuilder;
typedef struct WhereMaskSet WhereMaskSet;
typedef struct WhereMemBlock WhereMemBlock;
typedef struct WhereOrCost WhereOrCost;
typedef struct WhereOrInfo WhereOrInfo;
typedef struct WhereOrSet WhereOrSet;
typedef struct WherePath WherePath;
typedef struct WhereRightJoin WhereRightJoin;
typedef struct WhereScan WhereScan;
typedef struct WhereTerm WhereTerm;
typedef struct Window Window;
typedef struct WindowCodeArg WindowCodeArg;
typedef struct WindowCsrAndReg WindowCsrAndReg;
typedef struct WindowRewrite WindowRewrite;
typedef struct With With;
typedef struct yyParser yyParser;
typedef struct yyStackEntry yyStackEntry;
typedef u16 ht_slot;
typedef u64 tRowcnt;
typedef u64 uptr;
typedef unsigned bft;
typedef unsigned Bool;
typedef unsigned char etByte;
typedef void (*LOGFUNC_t)(void *, int, const char *);
typedef void (*void_function)(void);

typedef const sqlite3_io_methods *(*finder_type)(const char *, unixFile *);
typedef double double_t;
typedef float float_t;
typedef int (*RecordCompare)(int, const void *, UnpackedRecord *);
typedef int (*SorterCompare)(SortSubtask *, int *, const void *, int, const void *, int);
typedef int (*sqlite3_loadext_entry)(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pThunk);
typedef int (*sqlite3FaultFuncType)(int);
typedef int (*sqlite3LocaltimeType)(const void *, void *);

/*
 * Small structs that only ever appeared inline in the original amalgamation
 * (never split into their own container by the earlier struct-extraction
 * pass) still need a file-scope tag here. Otherwise the first header that
 * mentions them via a bare `struct Foo *` parameter -- rather than through
 * their container's own typedef -- creates its own incompatible
 * prototype-scope tag (C99 6.7.2.3p8), which then conflicts with the real
 * definition once both are pulled into the same translation unit.
 */
typedef struct Mem0Global Mem0Global;
typedef struct et_info et_info;
typedef struct unix_syscall unix_syscall;
typedef struct vxworksFileId vxworksFileId;
typedef struct PCacheGlobal PCacheGlobal;
typedef struct RowSetEntry RowSetEntry;
typedef struct RowSetChunk RowSetChunk;
typedef struct ValueNewStat4Ctx ValueNewStat4Ctx;
typedef struct ReusableSpace ReusableSpace;
typedef struct IdxCover IdxCover;
typedef struct RefSrcList RefSrcList;
typedef struct compareInfo compareInfo;
typedef struct TabResult TabResult;
typedef struct CallCount CallCount;
typedef struct NthValueCtx NthValueCtx;
typedef struct NtileCtx NtileCtx;
typedef struct LastValueCtx LastValueCtx;
typedef struct NanInfName NanInfName;
typedef struct WalWriter WalWriter;
typedef struct Percentile Percentile;
typedef struct SubstContext SubstContext;

#ifdef __cplusplus
}
#endif