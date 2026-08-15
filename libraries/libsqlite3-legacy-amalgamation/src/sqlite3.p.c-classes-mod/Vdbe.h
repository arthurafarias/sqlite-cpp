
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

  static Parse *sqlite3VdbeParser(Vdbe *);
  static int sqlite3VdbeAddOp0(Vdbe *, int);
  static int sqlite3VdbeAddOp1(Vdbe *, int, int);
  static int sqlite3VdbeAddOp2(Vdbe *, int, int, int);
  static int sqlite3VdbeGoto(Vdbe *, int);
  static int sqlite3VdbeLoadString(Vdbe *, int, const char *);
  static void sqlite3VdbeMultiLoad(Vdbe *, int, const char *, ...);
  static int sqlite3VdbeAddOp3(Vdbe *, int, int, int, int);
  static int sqlite3VdbeAddOp4(Vdbe *, int, int, int, int, const char *zP4, int);
  static int sqlite3VdbeAddOp4Dup8(Vdbe *, int, int, int, int, const u8 *, int);
  static int sqlite3VdbeAddOp4Int(Vdbe *, int, int, int, int, int);
  static void sqlite3VdbeEndCoroutine(Vdbe *, int);
  static VdbeOp *sqlite3VdbeAddOpList(Vdbe *, int nOp, VdbeOpList const *aOp, int iLineno);
  static void sqlite3VdbeAddParseSchemaOp(Vdbe *, int, char *, u16);
  static void sqlite3VdbeChangeOpcode(Vdbe *, int addr, u8);
  static void sqlite3VdbeChangeP1(Vdbe *, int addr, int P1);
  static void sqlite3VdbeChangeP2(Vdbe *, int addr, int P2);
  static void sqlite3VdbeChangeP3(Vdbe *, int addr, int P3);
  static void sqlite3VdbeChangeP5(Vdbe *, u16 P5);
  static void sqlite3VdbeTypeofColumn(Vdbe *, int);
  static void sqlite3VdbeJumpHere(Vdbe *, int addr);
  static void sqlite3VdbeJumpHereOrPopInst(Vdbe *, int addr);
  static int sqlite3VdbeChangeToNoop(Vdbe *, int addr);
  static int sqlite3VdbeDeletePriorOpcode(Vdbe *, u8 op);
  static void sqlite3VdbeChangeP4(Vdbe *, int addr, const char *zP4, int N);
  static void sqlite3VdbeAppendP4(Vdbe *, void *pP4, int p4type);
  static void sqlite3VdbeUsesBtree(Vdbe *, int);
  static VdbeOp *sqlite3VdbeGetOp(Vdbe *, int);
  static VdbeOp *sqlite3VdbeGetLastOp(Vdbe *);
  static void sqlite3VdbeRunOnlyOnce(Vdbe *);
  static void sqlite3VdbeReusable(Vdbe *);
  static void sqlite3VdbeDelete(Vdbe *);
  static void sqlite3VdbeMakeReady(Vdbe *, Parse *);
  static int sqlite3VdbeFinalize(Vdbe *);
  static void sqlite3VdbeResolveLabel(Vdbe *, int);
  static int sqlite3VdbeCurrentAddr(Vdbe *);
  static void sqlite3VdbeResetStepResult(Vdbe *);
  static void sqlite3VdbeRewind(Vdbe *);
  static int sqlite3VdbeReset(Vdbe *);
  static void sqlite3VdbeSetNumCols(Vdbe *, int);
  static int sqlite3VdbeSetColName(Vdbe *, int, int, const char *, void (*)(void *));
  static void sqlite3VdbeCountChanges(Vdbe *);
  static sqlite3 *sqlite3VdbeDb(Vdbe *);
  static u8 sqlite3VdbePrepareFlags(Vdbe *);
  static void sqlite3VdbeSetSql(Vdbe *, const char *z, int n, u8);
  static void sqlite3VdbeSwap(Vdbe *, Vdbe *);
  static VdbeOp *sqlite3VdbeTakeOpArray(Vdbe *, int *, int *);
  static sqlite3_value *sqlite3VdbeGetBoundValue(Vdbe *, int, u8);
  static void sqlite3VdbeSetVarmask(Vdbe *, int);
  static char *sqlite3VdbeExpandSql(Vdbe *, const char *);

#ifdef __cplusplus
}
#endif
