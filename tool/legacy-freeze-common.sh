# Shared file-enumeration logic for tool/check-legacy-freeze.sh and
# tool/update-legacy-freeze-manifest.sh (SRS 002 RR-6/TR-4). Sourced, not
# executed directly.
#
# Lists every file under the frozen legacy-track directories
# (libraries/libsqlite3-legacy/, applications/*-legacy/), excluding each
# directory's own CMakeLists.txt/cmake/* build glue -- the "minimal
# build-glue changes RR-1/RR-2/RR-4 themselves require" carve-out RR-6
# itself permits. Paths are printed relative to the given repo top, sorted,
# so the manifest is stable and diffable.
legacy_freeze_files() {
    local top="$1"
    local dirs=("$top/libraries/libsqlite3-legacy")
    local d
    for d in "$top"/applications/*-legacy; do
        [ -d "$d" ] && dirs+=("$d")
    done
    find "${dirs[@]}" -type f \
        ! -name 'CMakeLists.txt' \
        ! -path '*/cmake/*' \
        | sed "s#^$top/##" \
        | sort
}
