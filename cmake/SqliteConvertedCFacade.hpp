#pragma once

// Temporary ABI bridge for SRS-001 C libraries that consume converted leaves.
// Keeping it outside libraries/ makes the compatibility boundary explicit and
// lets converted sources remain free of C-linkage declarations. Remove this
// façade after the last dependent has crossed the SRS-002 boundary.
extern "C" {
#include "sqliteInt.h"
#include "vdbeInt.h"
}
