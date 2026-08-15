#pragma once

#include "Fts5ExtensionApi.h"
#include "Fts5Context.h"
#include "sqlite3_context.h"
#include "sqlite3_value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*fts5_extension_function)(const Fts5ExtensionApi *pApi, Fts5Context *pFts, sqlite3_context *pCtx, int nVal, sqlite3_value **apVal);

#ifdef __cplusplus
}
#endif