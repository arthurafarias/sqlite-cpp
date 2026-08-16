#include "sqlite/_All.h"

char *sqlite3ColumnType(Column *pCol, char *zDflt) {
  if (pCol->colFlags & 0x0004) {
    return pCol->zCnName + strlen(pCol->zCnName) + 1;
  } else if (pCol->eCType) {

    ((void)(0))

        ;
    return (char *)sqlite3StdType[pCol->eCType - 1];
  } else {
    return zDflt;
  }
}

const Mem *columnNullValue(void) {

  static const Mem nullMem

      = {
          {0}, (char *)0, (int)0, (u16)0x0001, (u8)0, (u8)0, (sqlite3 *)0, (int)0, (u32)0, (char *)0, (void (*)(void *))0,

      };
  return &nullMem;
}

const char *sqlite3ColumnColl(Column *pCol) {
  const char *z;
  if ((pCol->colFlags & 0x0200) == 0)
    return 0;
  z = pCol->zCnName;
  while (*z) {
    z++;
  }
  if (pCol->colFlags & 0x0004) {
    do {
      z++;
    } while (*z);
  }
  return z + 1;
}
