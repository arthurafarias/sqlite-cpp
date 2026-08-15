
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct WhereMaskSet WhereMaskSet;
struct WhereMaskSet {
  int bVarSelect;
  int n;
  int ix[((int)(sizeof(Bitmask) * 8))];
};

#ifdef __cplusplus
}
#endif

