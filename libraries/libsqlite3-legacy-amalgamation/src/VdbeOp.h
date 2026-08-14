#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <CollSeq.h>
#include <FuncDef.h>
#include <Index.h>
#include <KeyInfo.h>
#include <Mem.h>
#include <SubProgram.h>
#include <SubrtnSig.h>
#include <Table.h>
#include <VTable.h>
#include <i64.h>
#include <sqlite3_context.h>
#include <u16.h>
#include <u32.h>
#include <u8.h>

typedef struct VdbeOp VdbeOp;

struct VdbeOp {
  u8 opcode;
  signed char p4type;
  u16 p5;
  int p1;
  int p2;
  int p3;
  union p4union {
    int i;
    void *p;
    char *z;
    i64 *pI64;
    double *pReal;
    FuncDef *pFunc;
    sqlite3_context *pCtx;
    CollSeq *pColl;
    Mem *pMem;
    VTable *pVtab;
    KeyInfo *pKeyInfo;
    u32 *ai;
    SubProgram *pProgram;
    Table *pTab;
    SubrtnSig *pSubrtnSig;
    Index *pIdx;

  } p4;
};

#ifdef __cplusplus
}
#endif