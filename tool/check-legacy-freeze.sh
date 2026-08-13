#!/usr/bin/env bash
# SRS 002 TR-4: verifies no file under libraries/libsqlite3-legacy/ or any
# applications/*-legacy/ directory has changed since the last reviewed
# manifest update, other than through the RR-6 build-glue carve-out (each
# directory's own CMakeLists.txt/cmake/*, excluded by legacy_freeze_files).
# Registered as the `legacy_freeze_check` CTest test (BR-5).
#
# If a change here is an explicitly reviewed, documented exception (RR-6),
# rerun tool/update-legacy-freeze-manifest.sh and record why in the commit
# that does so -- that rerun *is* the review record.
set -euo pipefail

TOP="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=./legacy-freeze-common.sh
source "$TOP/tool/legacy-freeze-common.sh"

MANIFEST="$TOP/tests/legacy-freeze/manifest.sha256"
if [ ! -f "$MANIFEST" ]; then
    echo "error: legacy freeze manifest missing at $MANIFEST" >&2
    echo "run tool/update-legacy-freeze-manifest.sh to create it" >&2
    exit 1
fi

cd "$TOP"
CURRENT="$(mktemp)"
trap 'rm -f "$CURRENT"' EXIT
legacy_freeze_files "$TOP" | xargs -r sha256sum > "$CURRENT"

if ! diff -u "$MANIFEST" "$CURRENT"; then
    echo >&2
    echo "error: SRS 002 RR-6 freeze violated -- the legacy track has changed" >&2
    echo "since the last reviewed manifest update (see diff above)." >&2
    echo "If this is an explicitly reviewed, documented exception, rerun" >&2
    echo "tool/update-legacy-freeze-manifest.sh and note why in the commit." >&2
    exit 1
fi

echo "legacy freeze check passed: no unreviewed changes under libraries/libsqlite3-legacy or applications/*-legacy"
