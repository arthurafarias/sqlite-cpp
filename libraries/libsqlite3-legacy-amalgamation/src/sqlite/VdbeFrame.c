#include "sqlite/_All.h"

void sqlite3VdbeFrameMemDel(void *pArg) {
  VdbeFrame *pFrame = (VdbeFrame *)pArg;

  pFrame->pParent = pFrame->v->pDelFrame;
  pFrame->v->pDelFrame = pFrame;
}

void sqlite3VdbeFrameDelete(VdbeFrame *p) {
  int i;
  Mem *aMem = ((Mem *)&((u8 *)p)[(((sizeof(VdbeFrame)) + 7) & ~7)]);
  VdbeCursor **apCsr = (VdbeCursor **)&aMem[p->nChildMem];

  for (i = 0; i < p->nChildCsr; i++) {
    if (apCsr[i])
      sqlite3VdbeFreeCursorNN(p->v, apCsr[i]);
  }
  releaseMemArray(aMem, p->nChildMem);
  sqlite3VdbeDeleteAuxData(p->v->db, &p->pAuxData, -1, 0);
  sqlite3DbFree(p->v->db, p);
}

int sqlite3VdbeFrameRestore(VdbeFrame *pFrame) {
  Vdbe *v = pFrame->v;
  closeCursorsInFrame(v);
  v->aOp = pFrame->aOp;
  v->nOp = pFrame->nOp;
  v->aMem = pFrame->aMem;
  v->nMem = pFrame->nMem;
  v->apCsr = pFrame->apCsr;
  v->nCursor = pFrame->nCursor;
  v->db->lastRowid = pFrame->lastRowid;
  v->nChange = pFrame->nChange;
  v->db->nChange = pFrame->nDbChange;
  sqlite3VdbeDeleteAuxData(v->db, &v->pAuxData, -1, 0);
  v->pAuxData = pFrame->pAuxData;
  pFrame->pAuxData = 0;
  return pFrame->pc;
}
