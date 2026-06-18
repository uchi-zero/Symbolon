# /// script
# requires-python = ">=3.12"
# ///
"""Extract end-to-end metrics from the symbolon and baseline KLEE sweeps.

Reads each per-run `1.tar.gz` directly (no decompression to disk), pulling
coverage from `manifest.json` and resource counters from `klee-out/run.stats`
(a SQLite time-series, queried via the `sqlite3` CLI). Emits one CSV per metric
to the results dir, keyed by (program, strategy, system).

The evaluation/result directories are configured via the SYMBOLON_EVAL_ROOT,
SYMBOLON_BASELINE_ROOT, and SYMBOLON_RESULTS_DIR environment variables (with the
defaults below relative to ./evaluations).
"""

import csv
import json
import os
import subprocess
import sys
import tarfile
import tempfile
from collections import defaultdict

SYM_ROOT = os.environ.get("SYMBOLON_EVAL_ROOT", "evaluations/symbolon")
BASE_ROOT = os.environ.get("SYMBOLON_BASELINE_ROOT", "evaluations/baselines")
OUT_DIR = os.environ.get("SYMBOLON_RESULTS_DIR", "evaluations/results")

SYSTEMS = [("symbolon", SYM_ROOT), ("baseline", BASE_ROOT)]

# symbolon `libtiff` and baseline `tiffinfo` are the same program -> one name.
PROGRAM_ALIAS = {"libtiff": "tiffinfo"}

# Collapse run.stats to scalars, mirroring skills/evaluation reference SQL:
# cumulative metrics take the final row; instantaneous take MAX().
RUN_STATS_SQL = """
SELECT
  MAX(NumStates)                                                  AS states_peak,
  MAX(MallocUsage)/1048576.0                                      AS mem_peak_mib,
  (SELECT States      FROM stats ORDER BY WallTime DESC LIMIT 1)  AS forks_total,
  (SELECT Queries     FROM stats ORDER BY WallTime DESC LIMIT 1)  AS queries_total,
  (SELECT QueryTime/1000000.0  FROM stats ORDER BY WallTime DESC LIMIT 1) AS query_s,
  (SELECT SolverTime/1000000.0 FROM stats ORDER BY WallTime DESC LIMIT 1) AS solver_s,
  (SELECT ForkTime/1000000.0   FROM stats ORDER BY WallTime DESC LIMIT 1) AS fork_s,
  (SELECT WallTime/1000000.0   FROM stats ORDER BY WallTime DESC LIMIT 1) AS elapsed_s
FROM stats;
"""

RUN_STATS_COLS = [
    "states_peak", "mem_peak_mib", "forks_total", "queries_total",
    "query_s", "solver_s", "fork_s", "elapsed_s",
]


def parse_max_time_hours(val):
    """'6h' / '12h' / '720m' / '3600' -> float hours; None on failure."""
    if val is None:
        return None
    s = str(val).strip().lower()
    try:
        if s.endswith("h"):
            return float(s[:-1])
        if s.endswith("m"):
            return float(s[:-1]) / 60.0
        if s.endswith("s"):
            return float(s[:-1]) / 3600.0
        return float(s) / 3600.0  # bare number = seconds
    except ValueError:
        return None


def read_archive(tarpath):
    """Open one run archive; return (manifest dict|None, run_stats_scalars|None,
    ktests int). run_stats_scalars is None when run.stats is absent/unreadable."""
    manifest = None
    run_stats = None
    ktests = 0
    with tarfile.open(tarpath, "r:gz") as tf:
        stats_member = None
        for m in tf.getmembers():
            name = m.name
            if name.endswith("manifest.json"):
                try:
                    manifest = json.load(tf.extractfile(m))
                except Exception:
                    manifest = None
            elif name.endswith("klee-out/run.stats"):
                stats_member = m
            elif name.endswith(".ktest") and "/test" in name:
                ktests += 1
        if stats_member is not None:
            run_stats = query_run_stats(tf, stats_member)
    return manifest, run_stats, ktests


def query_run_stats(tf, member):
    """Extract run.stats to a temp file and run the scalar-summary query."""
    try:
        with tempfile.NamedTemporaryFile(suffix=".stats", delete=False) as tmp:
            tmp.write(tf.extractfile(member).read())
            tmppath = tmp.name
    except Exception:
        return None
    try:
        out = subprocess.run(
            ["sqlite3", "-csv", tmppath, RUN_STATS_SQL],
            capture_output=True, text=True, timeout=120,
        )
        if out.returncode != 0 or not out.stdout.strip():
            return None
        row = next(csv.reader([out.stdout.strip().splitlines()[-1]]))
        vals = {}
        for col, raw in zip(RUN_STATS_COLS, row):
            raw = raw.strip()
            if raw == "":
                vals[col] = None
                continue
            f = float(raw)
            # integer-kind columns -> int; time/mem -> rounded float
            if col in ("states_peak", "forks_total", "queries_total"):
                vals[col] = int(round(f))
            elif col == "mem_peak_mib":
                vals[col] = round(f, 1)
            else:
                vals[col] = round(f, 2)
        return vals
    except Exception:
        return None
    finally:
        try:
            os.unlink(tmppath)
        except OSError:
            pass


def cov_block(manifest):
    return (manifest or {}).get("coverage") or {}


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    # rows[(program, strategy, system)] = record dict
    rows = {}
    # program-level coverage totals per (system, program, metric) for borrowing.
    totals = defaultdict(dict)  # totals[system][(program, total_key)] = value
    summary = {
        "archives": 0,
        "missing_manifest": [],
        "empty_coverage": defaultdict(list),   # system -> [prog/strat]
        "missing_run_stats": [],               # system/prog/strat
        "excluded_programs": defaultdict(list),  # system -> [prog]
    }

    COV_METRICS = {
        "lines": ("lines-covered", "lines-total"),
        "functions": ("functions-covered", "functions-total"),
        "branches": ("branches-covered", "branches-total"),
    }

    # ---- Pass 1: read everything, record raw values + program totals ----
    sym_programs = set(os.listdir(SYM_ROOT))
    base_programs = set(os.listdir(BASE_ROOT))
    # canonical program names after alias merge
    sym_canon = {PROGRAM_ALIAS.get(p, p) for p in sym_programs if os.path.isdir(os.path.join(SYM_ROOT, p))}
    base_canon = {PROGRAM_ALIAS.get(p, p) for p in base_programs if os.path.isdir(os.path.join(BASE_ROOT, p))}
    common = sorted(sym_canon & base_canon)
    sym_only = sorted(sym_canon - base_canon)
    base_only = sorted(base_canon - sym_canon)
    for p in sym_only:
        summary["excluded_programs"]["symbolon"].append(p)
    for p in base_only:
        summary["excluded_programs"]["baseline"].append(p)

    for system, root in SYSTEMS:
        for prog_raw in sorted(os.listdir(root)):
            pdir = os.path.join(root, prog_raw)
            if not os.path.isdir(pdir):
                continue
            program = PROGRAM_ALIAS.get(prog_raw, prog_raw)
            if program not in common:
                continue  # excluded (no counterpart)
            for strat in sorted(os.listdir(pdir)):
                tarpath = os.path.join(pdir, strat, "1.tar.gz")
                if not os.path.exists(tarpath):
                    continue
                summary["archives"] += 1
                manifest, run_stats, ktests = read_archive(tarpath)
                key = (program, strat, system)
                cov = cov_block(manifest)

                max_time_h = parse_max_time_hours(
                    (manifest or {}).get("runtime", {}).get("max-time"))
                rec = {"program": program, "strategy": strat, "system": system,
                       "ktests": ktests, "run_stats": run_stats,
                       "max_time_h": max_time_h}
                # raw coverage (None if missing -> 0-fill later)
                for metric, (ck, tk) in COV_METRICS.items():
                    c = cov.get(ck)
                    t = cov.get(tk)
                    rec[f"{metric}_covered_raw"] = c
                    rec[f"{metric}_total_raw"] = t
                    # accumulate a program-level total (max non-null seen)
                    if t:
                        prev = totals[system].get((program, metric))
                        if prev is None or t > prev:
                            totals[system][(program, metric)] = t
                rows[key] = rec

                if manifest is None:
                    summary["missing_manifest"].append(f"{system}/{prog_raw}/{strat}")
                elif not cov or not cov.get("lines-total"):
                    summary["empty_coverage"][system].append(f"{prog_raw}/{strat}")
                if run_stats is None:
                    summary["missing_run_stats"].append(f"{system}/{prog_raw}/{strat}")

    # ---- Resolve coverage denominators (borrow within side, then cross-side) ----
    def resolve_total(system, program, metric, raw_total):
        if raw_total:
            return raw_total
        t = totals[system].get((program, metric))
        if t:
            return t
        other = "baseline" if system == "symbolon" else "symbolon"
        return totals[other].get((program, metric)) or 0

    # ---- Build per-metric CSV writers ----
    def pct(c, t):
        # fixed 2-decimal string so the column is visually uniform (0.00, 14.00)
        return f"{(100.0 * c / t):.2f}" if t else "0.00"

    sorted_keys = sorted(rows.keys(),
                         key=lambda k: (k[0], k[1], 0 if k[2] == "symbolon" else 1))

    def write_csv(fname, header, row_fn):
        path = os.path.join(OUT_DIR, fname)
        n = 0
        with open(path, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(header)
            for key in sorted_keys:
                w.writerow(row_fn(rows[key]))
                n += 1
        return path, n

    written = []

    # Coverage CSVs
    for metric, colbase in [("lines", "line"), ("functions", "function"), ("branches", "branch")]:
        cov_col = f"{colbase}_pct"
        covered_col = f"{metric}_covered"
        total_col = f"{metric}_total"
        fname = f"{colbase}_coverage.csv"

        def cov_row(rec, metric=metric, covered_col=covered_col, total_col=total_col, cov_col=cov_col):
            c = rec[f"{metric}_covered_raw"] or 0  # missing/empty -> 0
            t = resolve_total(rec["system"], rec["program"], metric,
                              rec[f"{metric}_total_raw"])
            return [rec["program"], rec["strategy"], rec["system"], c, t, pct(c, t)]

        header = ["program", "strategy", "system", covered_col, total_col, cov_col]
        written.append(write_csv(fname, header, cov_row))

    # Runtime scalar CSVs (NA when run.stats missing)
    def rs_val(rec, col):
        rs = rec["run_stats"]
        if rs is None:
            return ""  # NA
        v = rs.get(col)
        return "" if v is None else v

    runtime_specs = [
        ("states_peak.csv", "states_peak"),
        ("mem_peak_mib.csv", "mem_peak_mib"),
        ("forks_total.csv", "forks_total"),
        ("queries_total.csv", "queries_total"),
        ("query_s.csv", "query_s"),
        ("solver_s.csv", "solver_s"),
        ("fork_s.csv", "fork_s"),
    ]
    for fname, col in runtime_specs:
        header = ["program", "strategy", "system", col]
        written.append(write_csv(
            fname, header,
            lambda rec, col=col: [rec["program"], rec["strategy"], rec["system"], rs_val(rec, col)]))

    # ktests.csv (a real count even when run.stats is missing)
    written.append(write_csv(
        "ktests.csv", ["program", "strategy", "system", "ktests"],
        lambda rec: [rec["program"], rec["strategy"], rec["system"], rec["ktests"]]))

    # elapsed.csv: elapsed_s (from run.stats), max_time_h (manifest), hit_budget
    def elapsed_row2(rec):
        rs = rec["run_stats"]
        elapsed = "" if rs is None or rs.get("elapsed_s") is None else rs["elapsed_s"]
        mth = rec.get("max_time_h")
        if elapsed != "" and mth:
            hit = 1 if elapsed >= 0.98 * mth * 3600 else 0
        else:
            hit = ""
        return [rec["program"], rec["strategy"], rec["system"],
                elapsed, mth if mth is not None else "", hit]

    written.append(write_csv(
        "elapsed.csv",
        ["program", "strategy", "system", "elapsed_s", "max_time_h", "hit_budget"],
        elapsed_row2))

    # ---- Console summary ----
    print(f"Output dir: {OUT_DIR}")
    print(f"Common programs ({len(common)}): {', '.join(common)}")
    print(f"Archives processed: {summary['archives']}")
    print()
    print("CSV files written (rows = data rows, excl. header):")
    for path, n in written:
        print(f"  {os.path.basename(path):24} {n:4d} rows")
    print()
    print(f"Missing manifest (counts as 0 coverage): {len(summary['missing_manifest'])}")
    for x in summary["missing_manifest"]:
        print(f"    {x}")
    for system in ("symbolon", "baseline"):
        ec = summary["empty_coverage"][system]
        print(f"Empty coverage 0-filled [{system}]: {len(ec)}")
        for x in ec:
            print(f"    {x}")
    print(f"Missing run.stats (runtime NA): {len(summary['missing_run_stats'])}")
    for x in summary["missing_run_stats"]:
        print(f"    {x}")
    for system in ("symbolon", "baseline"):
        ex = summary["excluded_programs"][system]
        if ex:
            print(f"Excluded {system}-only programs (no counterpart): {', '.join(ex)}")


if __name__ == "__main__":
    main()
