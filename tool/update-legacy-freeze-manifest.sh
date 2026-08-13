#!/usr/bin/env bash
# SRS 002 RR-6/TR-4: regenerates tests/legacy-freeze/manifest.sha256, the
# baseline tool/check-legacy-freeze.sh diffs against. Running this script is
# the mechanical form of "an explicitly reviewed, documented exception" --
# only run it after a deliberate, reviewed change to the legacy track's
# build glue or (in an exceptional, documented case) its source, and say why
# in the commit that does so.
set -euo pipefail

TOP="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=./legacy-freeze-common.sh
source "$TOP/tool/legacy-freeze-common.sh"

MANIFEST="$TOP/tests/legacy-freeze/manifest.sha256"
mkdir -p "$(dirname "$MANIFEST")"

cd "$TOP"
legacy_freeze_files "$TOP" | xargs -r sha256sum > "$MANIFEST"

echo "wrote $(wc -l < "$MANIFEST") entries to $MANIFEST"
