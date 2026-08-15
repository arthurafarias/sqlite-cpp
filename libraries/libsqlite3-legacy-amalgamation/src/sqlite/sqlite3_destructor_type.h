#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite/_TypeIndex.h"

typedef void (*sqlite3_destructor_type)(void *);

#ifdef __cplusplus
}
#endif