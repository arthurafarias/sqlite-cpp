
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "AggInfo.h"
#include "analysisInfo.h"
#include "AuthContext.h"
#include "AutoincInfo.h"
#include "AuxData.h"
#include "BenignMallocHooks.h"
#include "BitMask.h"
#include "Bitvec.h"
#include "BtCursor.h"
#include "BtLock.h"
#include "Btree.h"
#include "BtreePayload.h"
#include "BtShared.h"
#include "BusyHandler.h"
#include "CellArray.h"
#include "CellInfo.h"
#include "CheckOnCtx.h"
#include "CollSeq.h"
#include "Column.h"
#include "CountCtx.h"
#include "CoveringIndexCheck.h"
#include "Cte.h"
#include "CteUse.h"
#include "DateTime.h"
#include "DbClientData.h"
#include "DbFixer.h"
#include "Db.h"
#include "DblquoteStr.h"
#include "DbPage.h"
#include "DbPath.h"
#include "DistinctCtx.h"
#include "EdupBuf.h"
#include "Expr.h"
#include "ExprList.h"
#include "FileChunk.h"
#include "FilePoint.h"
#include "FKey.h"
#include "FpDecode.h"
#include "fts5_api.h"
#include "Fts5Context.h"
#include "Fts5ExtensionApi.h"
#include "fts5_extension_function.h"
#include "Fts5PhraseIter.h"
#include "fts5_tokenizer.h"
#include "Fts5Tokenizer.h"
#include "fts5_tokenizer_v2.h"
#include "FuncDef.h"
#include "FuncDefHash.h"
#include "FuncDestructor.h"
#include "HashElem.h"
#include "Hash.h"
#include "HiddenIndexInfo.h"
#include "i16.h"
#include "i32.h"
#include "i64.h"
#include "i8.h"
#include "IdList.h"
#include "Incrblob.h"
#include "IncrMerger.h"
#include "IndexedExpr.h"
#include "Index.h"
#include "IndexIterator.h"
#include "IndexListTerm.h"
#include "IndexSample.h"
#include "InitData.h"
#include "IntegrityCk.h"
#include "JsonCache.h"
#include "JsonEachConnection.h"
#include "JsonEachCursor.h"
#include "JsonParent.h"
#include "JsonParse.h"
#include "JsonPretty.h"
#include "JsonString.h"
#include "KeyClass.h"
#include "KeyInfo.h"
#include "Lookaside.h"
#include "LookasideSlot.h"
#include "MemFile.h"
#include "Mem.h"
#include "MemJournal.h"
#include "MemPage.h"
#include "MemStore.h"
#include "MergeEngine.h"
#include "Module.h"
#include "NameContext.h"
#include "OnOrUsing.h"
#include "Op.h"
#include "Pager.h"
#include "PagerSavepoint.h"
#include "ParseCleanup.h"
#include "Parse.h"
#include "PCache1.h"
#include "PCache.h"
#include "PgFreeslot.h"
#include "PgHdr1.h"
#include "PgHdr.h"
#include "Pgno.h"
#include "PGroup.h"
#include "PmaReader.h"
#include "PmaWriter.h"
#include "PragmaVtabCursor.h"
#include "PragmaVtab.h"
#include "PreUpdate.h"
#include "PrintfArguments.h"
#include "RCStr.h"
#include "RenameCtx.h"
#include "RenameToken.h"
#include "Returning.h"
#include "RowLoadInfo.h"
#include "RowSet.h"
#include "Savepoint.h"
#include "ScanStatus.h"
#include "Schema.h"
#include "SelectDest.h"
#include "Select.h"
#include "SortCtx.h"
#include "SorterFile.h"
#include "SorterList.h"
#include "SorterRecord.h"
#include "SortSubtask.h"
#include "sqlite3_api_routines.h"
#include "sqlite3AutoExtList.h"
#include "sqlite3_backup.h"
#include "sqlite3_blob.h"
#include "sqlite3_callback.h"
#include "Sqlite3Config.h"
#include "sqlite3_context.h"
#include "sqlite3_destructor_type.h"
#include "sqlite3_file.h"
#include "sqlite3_filename.h"
#include "sqlite3.h"
#include "sqlite3_hard_heap.h"
#include "sqlite3_index_info.h"
#include "sqlite3_int64.h"
#include "sqlite3_io_methods.h"
#include "sqlite3_libversion.h"
#include "sqlite3_libversion_number.h"
#include "sqlite3_mem_methods.h"
#include "sqlite3_module.h"
#include "sqlite3_mutex.h"
#include "sqlite3_mutex_methods.h"
#include "sqlite3_pcache.h"
#include "sqlite3_pcache_methods2.h"
#include "sqlite3_pcache_methods.h"
#include "sqlite3_pcache_page.h"
#include "sqlite3_rtree_dbl.h"
#include "sqlite3_rtree_geometry.h"
#include "sqlite3_rtree_query_info.h"
#include "sqlite3_snapshot.h"
#include "sqlite3_soft_heap.h"
#include "sqlite3_sourceid.h"
#include "sqlite3StatType.h"
#include "sqlite3_stmt.h"
#include "sqlite3_str.h"
#include "sqlite3_syscall_ptr.h"
#include "sqlite3_uint64.h"
#include "sqlite3_value.h"
#include "sqlite3_version.h"
#include "sqlite3_vfs.h"
#include "sqlite3_vtab_cursor.h"
#include "sqlite3_vtab.h"
#include "sqlite3_xauth.h"
#include "sqlite_int64.h"
#include "SQLiteThread.h"
#include "sqlite_uint64.h"
#include "SrcItem.h"
#include "SrcList.h"
#include "StatAccum.h"
#include "StatSample.h"
#include "StrAccum.h"
#include "SubProgram.h"
#include "Subquery.h"
#include "SubrtnSig.h"
#include "SumCtx.h"
#include "Table.h"
#include "TableLock.h"
#include "Token.h"
#include "TreeView.h"
#include "Trigger.h"
#include "TriggerPrg.h"
#include "TriggerStep.h"
#include "u16.h"
#include "u32.h"
#include "u64.h"
#include "u8.h"
#include "unixFile.h"
#include "unixInodeInfo.h"
#include "unixShm.h"
#include "unixShmNode.h"
#include "UnixUnusedFd.h"
#include "UnpackedRecord.h"
#include "Upsert.h"
#include "ValueList.h"
#include "VdbeCursor.h"
#include "VdbeFrame.h"
#include "Vdbe.h"
#include "VdbeOp.h"
#include "VdbeOpList.h"
#include "VdbeSorter.h"
#include "VdbeTxtBlbCache.h"
#include "VList.h"
#include "VtabCtx.h"
#include "VTable.h"
#include "WalCkptInfo.h"
#include "Wal.h"
#include "WalHashLoc.h"
#include "WalIndexHdr.h"
#include "WalIterator.h"
#include "Walker.h"
#include "WhereAndInfo.h"
#include "WhereClause.h"
#include "WhereConst.h"
#include "WhereInfo.h"
#include "WhereLevel.h"
#include "WhereLoopBuilder.h"
#include "WhereLoop.h"
#include "WhereMaskSet.h"
#include "WhereMemBlock.h"
#include "WhereOrCost.h"
#include "WhereOrInfo.h"
#include "WhereOrSet.h"
#include "WherePath.h"
#include "WhereRightJoin.h"
#include "WhereScan.h"
#include "WhereTerm.h"
#include "WindowCodeArg.h"
#include "WindowCsrAndReg.h"
#include "Window.h"
#include "WindowRewrite.h"
#include "With.h"
#include "yDbMask.h"
#include "ynVar.h"
#include "yyParser.h"

#include "FuncDef.h"
#include "Index.h"
#include "Vdbe.h"

  typedef struct Parse Parse;

  struct Parse {
    sqlite3 *db;
    char *zErrMsg;
    Vdbe *pVdbe;
    int rc;
    LogEst nQueryLoop;
    u8 nested;
    u8 nTempReg;
    u8 isMultiWrite;
    u8 disableLookaside;
    u8 prepFlags;
    u8 withinRJSubrtn;
    u8 mSubrtnSig;
    u8 eTriggerOp;
    u8 eOrconf;
    bft disableTriggers : 1;
    bft mayAbort : 1;
    bft hasCompound : 1;
    bft bReturning : 1;
    bft bHasExists : 1;
    bft colNamesSet : 1;
    bft bHasWith : 1;
    bft okConstFactor : 1;
    bft checkSchema : 1;
    int nRangeReg;
    int iRangeReg;
    int nErr;
    int nTab;
    int nMem;
    int szOpAlloc;
    int iSelfTab;

    int nNestSel;
    int nLabel;
    int nLabelAlloc;
    int *aLabel;
    ExprList *pConstExpr;
    IndexedExpr *pIdxEpr;
    IndexedExpr *pIdxPartExpr;
    yDbMask writeMask;
    yDbMask cookieMask;
    int nMaxArg;
    int nSelect;

    u32 nProgressSteps;

    int nTableLock;
    TableLock *aTableLock;

    AutoincInfo *pAinc;
    Parse *pToplevel;
    Table *pTriggerTab;
    TriggerPrg *pTriggerPrg;
    ParseCleanup *pCleanup;
    int aTempReg[8];
    Parse *pOuterParse;
    Token sNameToken;
    u32 oldmask;
    u32 newmask;
    union {
      struct {
        int addrCrTab;
        int regRowid;
        int regRoot;
        Token constraintName;
      } cr;
      struct {
        Returning *pReturning;
      } d;
    } u1;
    Token sLastToken;
    ynVar nVar;
    u8 iPkSortOrder;
    u8 explain;
    u8 eParseMode;

    int nVtabLock;

    int nHeight;
    int addrExplain;
    VList *pVList;
    Vdbe *pReprepare;
    const char *zTail;
    Table *pNewTable;
    Index *pNewIndex;

    Trigger *pNewTrigger;
    const char *zAuthContext;

    Token sArg;
    Table **apVtabLock;

    With *pWith;

    RenameToken *pRename;
  };
  Vdbe *sqlite3VdbeCreate(Parse *);
  int sqlite3VdbeAddFunctionCall(Parse *, int, int, int, int, const FuncDef *, int);
  int sqlite3VdbeExplain(Parse *, u8, const char *, ...);
  void sqlite3VdbeExplainPop(Parse *);
  int sqlite3VdbeExplainParent(Parse *);
  void sqlite3VdbeSetP4KeyInfo(Parse *, Index *);
  int sqlite3VdbeMakeLabel(Parse *);
  Window *sqlite3WindowAlloc(Parse *, int, int, Expr *, int, Expr *, u8);
  void sqlite3WindowAttach(Parse *, Expr *, Window *);
  void sqlite3WindowCodeInit(Parse *, Select *);
  void sqlite3WindowCodeStep(Parse *, Select *, WhereInfo *, int, int);
  int sqlite3WindowRewrite(Parse *, Select *);
  void sqlite3WindowUpdate(Parse *, Window *, Window *, FuncDef *);
  void sqlite3WindowChain(Parse *, Window *, Window *);
  Window *sqlite3WindowAssemble(Parse *, Window *, ExprList *, ExprList *, Token *);
  int sqlite3RunParser(Parse *, const char *);
void sqlite3FinishCoding(Parse *);
int sqlite3GetTempReg(Parse *);
void sqlite3ReleaseTempReg(Parse *, int);
int sqlite3GetTempRange(Parse *, int);
void sqlite3ReleaseTempRange(Parse *, int, int);
void sqlite3ClearTempRegCache(Parse *);
void sqlite3TouchRegister(Parse *, int);
Expr *sqlite3PExpr(Parse *, int, Expr *, Expr *);
void sqlite3PExprAddSelect(Parse *, Expr *, Select *);
Expr *sqlite3ExprAnd(Parse *, Expr *, Expr *);
Expr *sqlite3ExprFunction(Parse *, ExprList *, const Token *, int);
void sqlite3ExprAddFunctionOrderBy(Parse *, Expr *, ExprList *);
void sqlite3ExprOrderByAggregateError(Parse *, Expr *);
void sqlite3ExprFunctionUsable(Parse *, const Expr *, const FuncDef *);
void sqlite3ExprAssignVarNumber(Parse *, Expr *, u32);
int sqlite3ExprDeferredDelete(Parse *, Expr *);
void sqlite3ExprUnmapAndDelete(Parse *, Expr *);
ExprList *sqlite3ExprListAppend(Parse *, ExprList *, Expr *);
ExprList *sqlite3ExprListAppendVector(Parse *, ExprList *, IdList *, Expr *);
Select *sqlite3ExprListToValues(Parse *, int, ExprList *);
void sqlite3ExprListSetName(Parse *, ExprList *, const Token *, int);
void sqlite3ExprListSetSpan(Parse *, ExprList *, const char *, const char *);

#ifdef __cplusplus
}
#endif
