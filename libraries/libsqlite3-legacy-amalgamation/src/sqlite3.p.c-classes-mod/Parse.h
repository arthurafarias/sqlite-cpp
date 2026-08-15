
#pragma once
#ifdef __cplusplus
extern C {
#endif

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
  static Vdbe *sqlite3VdbeCreate(Parse *);
  static int sqlite3VdbeAddFunctionCall(Parse *, int, int, int, int, const FuncDef *, int);
  static int sqlite3VdbeExplain(Parse *, u8, const char *, ...);
  static void sqlite3VdbeExplainPop(Parse *);
  static int sqlite3VdbeExplainParent(Parse *);
  static void sqlite3VdbeSetP4KeyInfo(Parse *, Index *);
  static int sqlite3VdbeMakeLabel(Parse *);

#ifdef __cplusplus
}
#endif
