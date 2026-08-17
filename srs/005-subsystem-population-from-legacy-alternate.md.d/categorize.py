#!/usr/bin/env python3
import re, json
from pathlib import Path
from collections import Counter

ROOT = Path("/home/arthur/Downloads/sqlite-cpp")
ALT_SRC = ROOT / "libraries/sqlite3-legacy-alternate/src/sqlite"
PRISTINE_SRC = ROOT / "libraries/sqlite3-legacy/pkg/sqlite-src-3530400/src"

FILE_TO_LIB = {}
def reg(lib, files):
    for f in files.split(","):
        f = f.strip()
        if f:
            FILE_TO_LIB[f] = lib

reg("sqlite3-utils", "mem0.c,mem1.c,mem2.c,mem3.c,mem5.c,malloc.c,util.c,printf.c,utf.c,hash.c,hash.h,mutex.c,mutex.h,random.c,status.c,fault.c,global.c")
reg("sqlite3-backend-os", "os.c,os.h,os_common.h,os_setup.h,os_unix.c,mutex_unix.c,os_win.c,os_win.h,mutex_w32.c,mutex_noop.c,os_kv.c,memdb.c")
reg("sqlite3-backend-pager", "pager.c,pager.h,wal.c,wal.h,pcache.c,pcache.h,pcache1.c,memjournal.c,bitvec.c")
reg("sqlite3-backend-tree", "btree.c,btree.h,btreeInt.h,btmutex.c,dbpage.c,dbstat.c")
reg("sqlite3-core-virtual-machine", "vdbe.c,vdbe.h,vdbeInt.h,opcodes.h,opcodes.c,vdbeapi.c,vdbeaux.c,vdbemem.c,vdbesort.c,vdbeblob.c,vdbetrace.c,vdbevtab.c")
reg("sqlite3-core-command-processor", "prepare.c,pragma.c,pragma.h,insert.c,update.c,delete.c,upsert.c,select.c,where.c,wherecode.c,whereexpr.c,whereInt.h,expr.c,resolve.c,walker.c,attach.c,alter.c,analyze.c,vacuum.c,table.c,rowset.c,build.c,trigger.c,window.c,fkey.c,auth.c,callback.c,treeview.c,func.c,json.c,date.c")
reg("sqlite3-core-interface", "main.c,legacy.c,loadext.c,sqlite3ext.h,vtab.c,backup.c,notify.c,threads.c,complete.c")
reg("sqlite3-compiler-tokenizer", "tokenize.c,keywordhash.h")
reg("sqlite3-compiler-parser", "parse.y,parse.c,parse.h")

CODE_GEN_ORIGIN = {"select.c","expr.c","insert.c","update.c","delete.c","trigger.c","where.c","wherecode.c"}

EXT_SKIP_PREFIXES = ("fts5","fts3","fts4","rtree","geopoly","session","rbu","icu","carray","closure",
                      "dbdata","series","regexp","spellfix","totype","userfault","unionvtab",
                      "wholenumber","zipfile","sqlar","amatch","csv","ieee754","vfslog","vfsstat",
                      "explain","fileio","shathree","decimal","memtrace","memvfs","mmapwarm","compress",
                      "adler32","anycollseq","appendvfs","atrc","base64","base85","btreecapi","cksumvfs",
                      "fuzzcheck","fuzzinvariants","qpvtab","scrub")
PRIMITIVE_TYPEDEFS = {"i8","i16","i32","i64","u8","u16","u32","u64","Pgno","LogEst","ynVar","yDbMask",
                       "sqlite_int64","sqlite_uint64","sqlite3_int64","sqlite3_uint64","tRowcnt","uptr"}

# High-confidence manual overrides: these are either LEMON parser-machinery
# (unambiguously compiler-parser), the public flag/enum surface of sqlite3.h
# (unambiguously core-interface), or cases the usage-count heuristic got
# outvoted on by an unrelated but more frequent user. Recorded here (not
# silently) precisely so the SRS's methodology section can list them as
# deliberate exceptions, mirroring sqlite3-legacy-alternate's own
# uncategorized-report.json practice for the same kind of judgment call.
MANUAL_OVERRIDES = {
    "yyParser": "sqlite3-compiler-parser",
    "YyAction": "sqlite3-compiler-parser",
    "YYMINORTYPE": "sqlite3-compiler-parser",
    "yyStackEntry": "sqlite3-compiler-parser",
    "WindowFunctionNames": "sqlite3-core-command-processor",
    "BitMask": "sqlite3-core-command-processor",
    "KeyClass": "sqlite3-core-command-processor",
    "FrameBound": "sqlite3-core-command-processor",
    "sqlite3_destructor_type": "sqlite3-core-interface",
    "sqlite3_filename": "sqlite3-core-interface",
    "sqlite3_hard_heap": "sqlite3-core-interface",
    "sqlite3_soft_heap": "sqlite3-core-interface",
    "sqlite3_loadext_entry": "sqlite3-core-interface",
    "sqlite3LocaltimeType": "sqlite3-core-command-processor",
    "sqlite3StatValueType": "sqlite3-backend-tree",
    "sqlite3FaultFuncType": "sqlite3-utils",
    "RecordCompare": "sqlite3-core-virtual-machine",
    "SorterCompare": "sqlite3-core-virtual-machine",
}
for _sym in ("SqliteAccessFlags","SqliteAuthorizerActionCode","SqliteAuthorizerReturnCode",
             "SqliteCheckpointMode","SqliteConfigOption","SqliteConflictResolution",
             "SqliteDbConfigOption","SqliteDbStatusParameter","SqliteDeserializeFlags",
             "SqliteDestructorType","SqliteExtendedResultCode","SqliteFileControlOpcode",
             "SqliteFunctionFlags","SqliteFundamentalDatatype","SqliteIndexConstraintOp",
             "SqliteIndexScanFlags","SqliteIoCap","SqliteLimitCategory","SqliteLockLevel",
             "SqliteMutexType","SqliteOpenFlags","SqlitePrepareFlags","SqliteResultCode",
             "SqliteScanStatusFlags","SqliteScanStatusOpcode","SqliteSerializeFlags",
             "SqliteSetlkTimeoutFlags","SqliteShmFlags","SqliteStatusParameter",
             "SqliteStmtStatusParameter","SqliteSyncFlags","SqliteTestCtrlOpcode",
             "SqliteTextEncoding","SqliteTraceEventCode","SqliteTxnState",
             "SqliteUnixSyscallIndex","SqliteVersion","SqliteVtabConfigOption",
             "SqliteWin32DirectoryType","TrigEvent"):
    MANUAL_OVERRIDES[_sym] = "sqlite3-core-interface"
for _sym in ("Bool","Lmid_t","LOGFUNC_t","long_double_t","void_function","ht_slot",
             "etByte","finder_type","aXformType_t"):
    MANUAL_OVERRIDES[_sym] = "sqlite3-utils"

def load_pristine_texts():
    texts = {}
    for p in PRISTINE_SRC.glob("*"):
        if p.is_file() and p.suffix in (".c", ".h", ".y"):
            try:
                texts[p.name] = p.read_text(errors="ignore")
            except Exception:
                pass
    return texts

def classify_symbol(symbol, texts, names):
    low = symbol.lower()
    if any(low.startswith(p) or ("_" + p) in low for p in EXT_SKIP_PREFIXES):
        return dict(decision="skip-extension", lib=None)
    if symbol in MANUAL_OVERRIDES:
        return dict(decision="manual-override", lib=MANUAL_OVERRIDES[symbol])
    if symbol in PRIMITIVE_TYPEDEFS:
        return dict(decision="fallback-primitive", lib="sqlite3-utils")

    pat = re.compile(r'(?<![A-Za-z0-9_])' + re.escape(symbol) + r'(?![A-Za-z0-9_])')
    counts = {}
    for name in names:
        n = len(pat.findall(texts[name]))
        if n:
            counts[name] = n
    if not counts:
        return dict(decision="unmatched", lib="sqlite3-utils", origin_files=[])
    ranked = sorted(counts.items(), key=lambda kv: -kv[1])
    mapped_ranked = [(n, c) for n, c in ranked if n in FILE_TO_LIB]
    if mapped_ranked:
        return dict(decision="matched-by-usage", lib=FILE_TO_LIB[mapped_ranked[0][0]],
                     origin_files=[mapped_ranked[0][0]], top_files=ranked[:5])
    return dict(decision="unmatched-only-unmapped-files", lib="sqlite3-utils",
                 origin_files=[ranked[0][0]], top_files=ranked[:5])

def main():
    texts = load_pristine_texts()
    names = list(texts.keys())

    c_files = {p.stem: p for p in ALT_SRC.glob("*.c")}
    h_files = {p.stem: p for p in ALT_SRC.glob("*.h")}
    all_symbols = sorted(set(c_files) | set(h_files))

    results = []
    for symbol in all_symbols:
        entry = {"symbol": symbol, "has_c": symbol in c_files, "has_h": symbol in h_files}
        entry.update(classify_symbol(symbol, texts, names))
        results.append(entry)

    # code-generator duplication: any symbol whose top origin file is one of
    # the eight files SRS-001 FR-3 already established as non-separable
    # between command-processor and code-generator gets copied into BOTH.
    for e in results:
        e["also_code_generator"] = bool(
            e.get("origin_files") and e["origin_files"][0] in CODE_GEN_ORIGIN
        )

    Path("/tmp/claude-1000/-home-arthur-Downloads-sqlite-cpp/825b629c-1793-4c31-aff9-ee8fc481916b/scratchpad/categorization3.json").write_text(json.dumps(results, indent=2))

    c = Counter(r["lib"] for r in results if r["lib"])
    print("Per-library counts (symbols, c+h merged):")
    for lib, n in sorted(c.items()):
        print(f"  {lib}: {n}")
    print("also_code_generator dup count:", sum(1 for r in results if r["also_code_generator"]))
    print("skip-extension:", sum(1 for r in results if r["decision"]=="skip-extension"))
    print("unmatched:", sum(1 for r in results if r["decision"] in ("unmatched","unmatched-only-unmapped-files")))
    print("total symbols:", len(results))

if __name__ == "__main__":
    main()
