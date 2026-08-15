
#pragma once
#include "sqlite3_value.h"
#ifdef __cplusplus
extern C {
#endif

#include "VdbeOpList.h"
#include "u16.h"
#include "u8.h"

  typedef struct Vdbe Vdbe;
  typedef struct Parse Parse;
  typedef struct VdbeOp VdbeOp;
  typedef struct sqlite3 sqlite3;
  typedef struct sqlite3_value sqlite3_value;


struct Vdbe {
  sqlite3 *db;
  Vdbe **ppVPrev, *pVNext;
  Parse *pParse;
  ynVar nVar;
  int nMem;
  int nCursor;
  u32 cacheCtr;
  int pc;
  int rc;
  i64 nChange;
  int iStatement;
  i64 iCurrentTime;
  i64 nFkConstraint;
  i64 nStmtDefCons;
  i64 nStmtDefImmCons;
  Mem *aMem;
  Mem **apArg;
  VdbeCursor **apCsr;
  Mem *aVar;

  Op *aOp;
  int nOp;
  int nOpAlloc;
  Mem *aColName;
  Mem *pResultRow;
  char *zErrMsg;
  VList *pVList;

  i64 startTime;

  u16 nResColumn;
  u16 nResAlloc;
  u8 errorAction;
  u8 minWriteFileFormat;
  u8 prepFlags;
  u8 eVdbeState;
  bft expired : 2;
  bft explain : 2;
  bft changeCntOn : 1;
  bft usesStmtJournal : 1;
  bft readOnly : 1;
  bft bIsReader : 1;
  bft haveEqpOps : 1;
  yDbMask btreeMask;
  yDbMask lockMask;
  u32 aCounter[9];
  char *zSql;

  void *pFree;
  VdbeFrame *pFrame;
  VdbeFrame *pDelFrame;
  int nFrame;
  u32 expmask;
  SubProgram *pProgram;
  AuxData *pAuxData;
};

  Parse *sqlite3VdbeParser(Vdbe *);
  int sqlite3VdbeAddOp0(Vdbe *, int);
  int sqlite3VdbeAddOp1(Vdbe *, int, int);
  int sqlite3VdbeAddOp2(Vdbe *, int, int, int);
  int sqlite3VdbeGoto(Vdbe *, int);
  int sqlite3VdbeLoadString(Vdbe *, int, const char *);
  void sqlite3VdbeMultiLoad(Vdbe *, int, const char *, ...);
  int sqlite3VdbeAddOp3(Vdbe *, int, int, int, int);
  int sqlite3VdbeAddOp4(Vdbe *, int, int, int, int, const char *zP4, int);
  int sqlite3VdbeAddOp4Dup8(Vdbe *, int, int, int, int, const u8 *, int);
  int sqlite3VdbeAddOp4Int(Vdbe *, int, int, int, int, int);
  void sqlite3VdbeEndCoroutine(Vdbe *, int);
  VdbeOp *sqlite3VdbeAddOpList(Vdbe *, int nOp, VdbeOpList const *aOp, int iLineno);
  void sqlite3VdbeAddParseSchemaOp(Vdbe *, int, char *, u16);
  void sqlite3VdbeChangeOpcode(Vdbe *, int addr, u8);
  void sqlite3VdbeChangeP1(Vdbe *, int addr, int P1);
  void sqlite3VdbeChangeP2(Vdbe *, int addr, int P2);
  void sqlite3VdbeChangeP3(Vdbe *, int addr, int P3);
  void sqlite3VdbeChangeP5(Vdbe *, u16 P5);
  void sqlite3VdbeTypeofColumn(Vdbe *, int);
  void sqlite3VdbeJumpHere(Vdbe *, int addr);
  void sqlite3VdbeJumpHereOrPopInst(Vdbe *, int addr);
  int sqlite3VdbeChangeToNoop(Vdbe *, int addr);
  int sqlite3VdbeDeletePriorOpcode(Vdbe *, u8 op);
  void sqlite3VdbeChangeP4(Vdbe *, int addr, const char *zP4, int N);
  void sqlite3VdbeAppendP4(Vdbe *, void *pP4, int p4type);
  void sqlite3VdbeUsesBtree(Vdbe *, int);
  VdbeOp *sqlite3VdbeGetOp(Vdbe *, int);
  VdbeOp *sqlite3VdbeGetLastOp(Vdbe *);
  void sqlite3VdbeRunOnlyOnce(Vdbe *);
  void sqlite3VdbeReusable(Vdbe *);
  void sqlite3VdbeDelete(Vdbe *);
  void sqlite3VdbeMakeReady(Vdbe *, Parse *);
  int sqlite3VdbeFinalize(Vdbe *);
  void sqlite3VdbeResolveLabel(Vdbe *, int);
  int sqlite3VdbeCurrentAddr(Vdbe *);
  void sqlite3VdbeResetStepResult(Vdbe *);
  void sqlite3VdbeRewind(Vdbe *);
  int sqlite3VdbeReset(Vdbe *);
  void sqlite3VdbeSetNumCols(Vdbe *, int);
  int sqlite3VdbeSetColName(Vdbe *, int, int, const char *, void (*)(void *));
  void sqlite3VdbeCountChanges(Vdbe *);
  sqlite3 *sqlite3VdbeDb(Vdbe *);
  u8 sqlite3VdbePrepareFlags(Vdbe *);
  void sqlite3VdbeSetSql(Vdbe *, const char *z, int n, u8);
  void sqlite3VdbeSwap(Vdbe *, Vdbe *);
  VdbeOp *sqlite3VdbeTakeOpArray(Vdbe *, int *, int *);
  sqlite3_value *sqlite3VdbeGetBoundValue(Vdbe *, int, u8);
  void sqlite3VdbeSetVarmask(Vdbe *, int);
  char *sqlite3VdbeExpandSql(Vdbe *, const char *);
  void sqlite3VdbeLinkSubProgram(Vdbe *, SubProgram *);
int sqlite3VdbeHasSubProgram(Vdbe *);
void sqlite3CodeChangeCount(Vdbe *, int, const char *);
void sqlite3WhereMinMaxOptEarlyOut(Vdbe *, WhereInfo *);
void sqlite3ExprCodeGetColumnOfTable(Vdbe *, Table *, int, int, int);
void sqlite3TableAffinity(Vdbe *, Table *, int);
void sqlite3ColumnDefault(Vdbe *, Table *, int, int);
void sqlite3VtabImportErrmsg(Vdbe *, sqlite3_vtab *);
int sqlite3VdbeParameterIndex(Vdbe *, const char *, int);
int sqlite3Reprepare(Vdbe *);
void sqlite3VdbeError(Vdbe *, const char *, ...);
void sqlite3VdbeFreeCursor(Vdbe *, VdbeCursor *);
void sqlite3VdbeFreeCursorNN(Vdbe *, VdbeCursor *);
void sqliteVdbePopStack(Vdbe *, int);
int sqlite3VdbeExec(Vdbe *);
int sqlite3VdbeNextOpcode(Vdbe *, Mem *, int, int *, int *, Op **);
int sqlite3VdbeList(Vdbe *);
int sqlite3VdbeHalt(Vdbe *);
int sqlite3VdbeCloseStatement(Vdbe *, int);
int sqlite3VdbeTransferError(Vdbe *p);
void sqlite3VdbeEnter(Vdbe *);
void sqlite3VdbeLeave(Vdbe *);
int sqlite3VdbeCheckFkImmediate(Vdbe *);
int sqlite3VdbeCheckFkDeferred(Vdbe *);

#ifdef __cplusplus
}
#endif
