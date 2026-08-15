
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct ValueList ValueList;
struct ValueList {
  BtCursor *pCsr;
  sqlite3_value *pOut;
};

#ifdef __cplusplus
}
#endif

