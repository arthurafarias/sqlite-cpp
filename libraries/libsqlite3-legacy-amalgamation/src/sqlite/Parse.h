
#pragma once
#ifdef __cplusplus
extern C {
#endif

#include "sqlite/u32.h"
#include "sqlite/u8.h"

#include "sqlite/AutoincInfo.h"
#include "sqlite/Token.h"

#include "sqlite/LogEst.h"
#include "sqlite/bft.h"
#include "sqlite/yDbMask.h"
#include "sqlite/TriggerPrg.h"
#include "sqlite/Returning.h"

  typedef struct sqlite3 sqlite3;
  typedef struct Vdbe Vdbe;
  typedef struct Parse Parse;
  
  typedef struct ExprList ExprList;
  typedef struct IndexedExpr IndexedExpr;
  typedef struct TableLock TableLock;
  typedef struct Table Table;


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
