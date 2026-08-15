
#pragma once
#ifdef __cplusplus
extern C {
#endif

typedef struct Trigger Trigger;

struct Trigger {
  char *zName;
  char *table;
  u8 op;
  u8 tr_tm;
  u8 bReturning;
  Expr *pWhen;
  IdList *pColumns;

  Schema *pSchema;
  Schema *pTabSchema;
  TriggerStep *step_list;
  Trigger *pNext;
};

#ifdef __cplusplus
}
#endif

