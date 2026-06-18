#!/usr/bin/env -S uv run
# /// script
# requires-python = ">=3.12"
# dependencies = [
#     "datasets>=3.0",
#     "typer>=0.15",
#     "lizard>=1.17",
#     "tenacity>=8.0",
# ]
# ///
"""Setup Code-Contests-Plus database with solutions and KLEE environments.

Usage:
    uv run scripts/code_contests_plus.py
    uv run scripts/code_contests_plus.py --db datasets/my.db --workers 16 --verbose
"""

import hashlib
import math
import multiprocessing as mp
import os
import re
import sqlite3
import sys
from collections import Counter
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Annotated

import lizard
import typer
from datasets import load_dataset
from tenacity import retry, stop_after_attempt, wait_exponential


class ProcessStatus(str, Enum):
    """Status codes for problem processing results."""
    OK = "ok"
    SKIP_NO_C_FAMILY = "skip_no_c_family"
    SKIP_BELOW_MIN = "skip_below_min"
    SKIP_NO_TESTS = "skip_no_tests"

# Language constants
LANG_C = "c"
LANG_CPP = "cpp"

# Multiprocessing constants
CHUNK_SIZE_MULTIPLIER = 4
BATCH_SIZE = 10000
PROGRESS_INTERVAL = 500

# C++ detection patterns (compiled regexes)
CPP_PATTERNS = [
    re.compile(
        r"#include\s*<("
        r"iostream|vector|algorithm|map|set|queue|stack|string|deque|list|"
        r"unordered_map|unordered_set|bitset|numeric|functional|tuple|array|"
        r"memory|utility|sstream|fstream|iomanip|regex|chrono|complex|"
        r"forward_list|priority_queue|valarray|ranges|span|optional|variant|"
        r"any|type_traits|concepts|format|print"
        r")>"
    ),
    re.compile(
        r"#include\s*<c(stdio|string|math|stdlib|type|assert|time|limits|float|errno)>"
    ),
    re.compile(r"\busing\s+namespace\b"),
    re.compile(r"\bstd::"),
    re.compile(r"\bcout\b"),
    re.compile(r"\bcin\b"),
    re.compile(r"\bcerr\b"),
    re.compile(r"\bendl\b"),
    re.compile(r"\btemplate\s*<"),
    re.compile(r"\bnullptr\b"),
    re.compile(r"\bclass\s+\w+\s*[:{]"),
    re.compile(r"\bvirtual\s"),
    re.compile(r"\bpublic\s*:|private\s*:|protected\s*:"),
    re.compile(r"\b(const|static|dynamic|reinterpret)_cast\s*<"),
    re.compile(r"\boperator\s*[+\-*/=<>!&|^~\[\]()]"),
    re.compile(r"\bnew\s+\w"),
    re.compile(r"\bdelete\s*[\[\w]"),
    re.compile(r"\w+\s*&{1,2}\s+\w+"),
]

C_FAMILY_PATTERN = re.compile(
    r"#include\s*[<\"]|"
    r"\bint\s+main\s*\(|"
    r"\bvoid\s+main\s*\(|"
    r"\b(int|char|float|double|long|short|unsigned|signed|void)\s+\w+\s*[;=(]|"
    r"#define\s|"
    r"\bprintf\s*\(|"
    r"\bscanf\s*\("
)


# C/C++ keywords for tokenization
C_KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while",
    "alignas", "alignof", "asm", "bool", "catch", "class", "concept",
    "consteval", "constexpr", "constinit", "const_cast", "co_await",
    "co_return", "co_yield", "decltype", "delete", "dynamic_cast",
    "explicit", "export", "false", "friend", "mutable", "namespace", "new",
    "noexcept", "nullptr", "operator", "override", "private", "protected",
    "public", "reinterpret_cast", "requires", "static_assert", "static_cast",
    "template", "this", "throw", "true", "try", "typeid", "typename",
    "using", "virtual", "wchar_t",
}

TOKEN_RE = re.compile(
    r'"(?:[^"\\]|\\.)*"'
    r"|'(?:[^'\\]|\\.)*'"
    r"|//[^\n]*"
    r"|/\*[\s\S]*?\*/"
    r"|#include\s*<[^>]*>"
    r'|#include\s*"[^"]*"'
    r"|#\w+"
    r"|\d[\d.]*(?:[eE][+-]?\d+)?[fFlLuU]*"
    r"|\w+"
    r"|[^\s]"
)

@dataclass
class Options:
    """Configuration options for the pipeline."""
    db: Path
    config: str
    min_solutions: int
    cluster_threshold: float
    workers: int
    verbose: bool


def is_c_family(code: str) -> bool:
    """Heuristic: returns True if code looks like C or C++."""
    if C_FAMILY_PATTERN.search(code[:500]):
        return True
    return bool(C_FAMILY_PATTERN.search(code))


def is_cpp(code: str) -> bool:
    """Returns True if code contains C++-specific patterns."""
    for pattern in CPP_PATTERNS:
        if pattern.search(code):
            return True
    return False


def tokenize_normalized(code: str) -> Counter[str]:
    """Tokenize C/C++ code and normalize identifiers/literals."""
    counts: Counter[str] = Counter()
    for tok in TOKEN_RE.findall(code):
        if tok.startswith("//") or tok.startswith("/*"):
            continue
        elif tok.startswith('"') or tok.startswith("'"):
            counts["$LIT"] += 1
        elif tok[0].isdigit():
            counts["$NUM"] += 1
        elif tok in C_KEYWORDS:
            counts[tok] += 1
        elif tok.startswith("#"):
            counts[tok] += 1
        elif tok[0].isalpha() or tok[0] == "_":
            counts["$ID"] += 1
        else:
            counts[tok] += 1
    return counts


def jaccard(a: Counter[str], b: Counter[str]) -> float:
    """Multiset Jaccard similarity: |A ∩ B| / |A ∪ B|."""
    intersection = sum(min(a[k], b[k]) for k in a.keys() & b.keys())
    union = sum(max(a[k], b.get(k, 0)) for k in a.keys() | b.keys())
    return intersection / union if union > 0 else 1.0


class UnionFind:
    """Union-Find data structure for clustering."""
    def __init__(self, n: int):
        self.parent = list(range(n))
        self.rank = [0] * n

    def find(self, x: int) -> int:
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def union(self, x: int, y: int) -> None:
        px, py = self.find(x), self.find(y)
        if px == py:
            return
        if self.rank[px] < self.rank[py]:
            px, py = py, px
        self.parent[py] = px
        if self.rank[px] == self.rank[py]:
            self.rank[px] += 1


def cluster_solutions(codes: list[str], threshold: float) -> list[int]:
    """Cluster solutions by token-level Jaccard similarity."""
    n = len(codes)
    if n == 0:
        return []

    tokens = [tokenize_normalized(code) for code in codes]
    uf = UnionFind(n)

    for a in range(n):
        for b in range(a + 1, n):
            if jaccard(tokens[a], tokens[b]) >= threshold:
                uf.union(a, b)

    root_to_cluster: dict[int, int] = {}
    cluster_ids = []
    for j in range(n):
        root = uf.find(j)
        if root not in root_to_cluster:
            root_to_cluster[root] = len(root_to_cluster)
        cluster_ids.append(root_to_cluster[root])

    return cluster_ids


def code_hash(code: str) -> str:
    """SHA-256 of structurally normalized code."""
    lines = [line.rstrip() for line in code.splitlines() if line.strip()]
    return hashlib.sha256("\n".join(lines).encode()).hexdigest()


def analyze_code(code: str, lang: str) -> dict:
    """Return metrics dict from lizard analysis."""
    filename = f"code.{lang}"
    analysis = lizard.analyze_file.analyze_source_code(filename, code)
    funcs = analysis.function_list
    return {
        "lines": analysis.nloc,
        "complexity": sum(f.cyclomatic_complexity for f in funcs) if funcs else 1,
        "max_nesting": max((f.max_nesting_depth for f in funcs), default=0),
    }


def generate_klee_env(test_cases: list[dict]) -> str | None:
    """Generate KLEE --sym-stdin flag sized from P95 of test case input lengths."""
    input_lengths = [len(t.get("input", "")) for t in test_cases]
    if not input_lengths or all(n == 0 for n in input_lengths):
        return None
    input_lengths.sort()
    idx = math.ceil(0.95 * len(input_lengths)) - 1
    p95 = input_lengths[max(idx, 0)]
    if p95 == 0:
        return None
    return f"--sym-stdin {p95}"


def build_problem_id(row: dict, split_name: str, idx: int) -> str:
    """Construct problem_id from dataset row."""
    source = row.get("source", "unknown")
    raw_id = row.get("id") or row.get("title") or str(idx)
    return f"{split_name}_{source}_{raw_id}"


def section(title: str, width: int = 60) -> None:
    """Print section header with horizontal rules."""
    print(f"\n{'=' * width}\n{title}\n{'=' * width}")


def init_phase(conn: sqlite3.Connection, cursor: sqlite3.Cursor,
               options: Options, verbose: bool) -> dict:
    """Initialize database schema. Idempotent."""

    cursor.execute("SELECT name FROM sqlite_master WHERE type='table'")
    existing_tables = {row[0] for row in cursor.fetchall()}

    expected_tables = {"problems", "solutions"}
    missing_tables = expected_tables - existing_tables

    if missing_tables:
        conn.executescript("""
            CREATE TABLE IF NOT EXISTS problems (
                problem_id    TEXT PRIMARY KEY,
                klee_sym_env  TEXT
            );

            CREATE TABLE IF NOT EXISTS solutions (
                problem_id    TEXT    NOT NULL,
                solution_id   INTEGER NOT NULL,
                code          TEXT    NOT NULL,
                language      TEXT    NOT NULL,
                correct       INTEGER NOT NULL,
                cluster_id    INTEGER NOT NULL,
                lines         INTEGER NOT NULL,
                complexity    INTEGER NOT NULL,
                max_nesting   INTEGER NOT NULL,
                PRIMARY KEY (problem_id, solution_id),
                FOREIGN KEY (problem_id) REFERENCES problems (problem_id)
            );

            CREATE INDEX IF NOT EXISTS idx_solutions_language
            ON solutions (language);

            CREATE INDEX IF NOT EXISTS idx_solutions_complexity
            ON solutions (complexity);
        """)
        conn.commit()

        if verbose:
            print("  Created tables:", ", ".join(sorted(missing_tables)))

    cursor.execute("SELECT name FROM sqlite_master WHERE type='index'")
    existing_indexes = {row[0] for row in cursor.fetchall()}
    expected_indexes = {"idx_solutions_language", "idx_solutions_complexity"}

    if not expected_indexes.issubset(existing_indexes):
        raise RuntimeError(
            f"Database schema invalid. Missing indexes: "
            f"{expected_indexes - existing_indexes}. "
            f"Delete {options.db} and re-run to recreate."
        )

    if not verbose:
        print("✓ Database initialized")
    else:
        print("  Tables:", ", ".join(sorted(existing_tables)))
        print("  Indexes:", ", ".join(sorted(existing_indexes & expected_indexes)))
        print("✓ Database initialized")

    return {"status": "ok"}


def _process_problem_worker(args: tuple) -> tuple[str, tuple[tuple[str, str], list[tuple], dict]] | tuple[str, None]:
    """Worker function that unpacks arguments and calls process_problem."""
    return process_problem(*args)


def process_problem(
    row: dict, idx: int, min_solutions: int, cluster_threshold: float,
    split_name: str, existing_problems: set[str]
) -> tuple[str, tuple[tuple[str, str], list[tuple], dict]] | tuple[str, None]:
    """Process a single problem: classify, deduplicate, cluster, analyze."""
    try:
        problem_id = build_problem_id(row, split_name, idx)

        # Skip if already in database (idempotency)
        if problem_id in existing_problems:
            return (ProcessStatus.SKIP_NO_C_FAMILY, None)

        test_cases = row.get("test_cases") or []
        klee_env = generate_klee_env(test_cases)
        if klee_env is None:
            return (ProcessStatus.SKIP_NO_TESTS, None)

        solutions: list[tuple[str, str, bool]] = []
        seen: set[str] = set()
        duplicates_removed = 0

        for correct, subs in [
            (True, row.get("correct_submissions") or []),
            (False, row.get("incorrect_submissions") or []),
        ]:
            for sub in subs:
                code = sub["code"].strip()
                if not is_c_family(code):
                    continue
                h = code_hash(code)
                if h in seen:
                    duplicates_removed += 1
                    continue
                seen.add(h)

                lang = LANG_CPP if is_cpp(code) else LANG_C
                solutions.append((code, lang, correct))

        if not solutions:
            return (ProcessStatus.SKIP_NO_C_FAMILY, None)

        if len(solutions) < min_solutions:
            return (ProcessStatus.SKIP_BELOW_MIN, None)

        codes = [code for code, _, _ in solutions]
        cluster_ids = cluster_solutions(codes, cluster_threshold)
        num_clusters = len(set(cluster_ids))

        problem_row = (problem_id, klee_env)
        solution_rows = []
        solutions_c = 0
        solutions_cpp = 0

        for sol_idx, ((code, lang, correct), cluster_id) in enumerate(
            zip(solutions, cluster_ids)
        ):
            m = analyze_code(code, lang)
            solution_rows.append(
                (
                    problem_id,
                    sol_idx,
                    code,
                    lang,
                    int(correct),
                    cluster_id,
                    m["lines"],
                    m["complexity"],
                    m["max_nesting"],
                )
            )
            if lang == LANG_C:
                solutions_c += 1
            else:
                solutions_cpp += 1

        stats = {
            "problems_written": 1,
            "problems_with_c": 1 if solutions_c > 0 else 0,
            "problems_with_cpp": 1 if solutions_cpp > 0 else 0,
            "solutions_c": solutions_c,
            "solutions_cpp": solutions_cpp,
            "clusters_total": num_clusters,
            "duplicates_removed": duplicates_removed,
            "problems_skipped_no_c_family": 0,
            "problems_skipped_below_min": 0,
        }

        return (ProcessStatus.OK, (problem_row, solution_rows, stats))
    except Exception as e:
        problem_id = build_problem_id(row, split_name, idx)
        typer.echo(f"Warning: Failed to process {problem_id}: {e}", err=True)
        return (ProcessStatus.SKIP_NO_C_FAMILY, None)


@retry(stop=stop_after_attempt(5), wait=wait_exponential(multiplier=1, max=16))
def load_dataset_with_retry(config: str):
    """Load dataset with tenacity retry."""
    return load_dataset("ByteDance-Seed/Code-Contests-Plus", config, streaming=True)


def download_phase(conn: sqlite3.Connection, cursor: sqlite3.Cursor,
                   options: Options, verbose: bool) -> dict:
    """Download and process solutions. Idempotent."""

    cursor.execute("SELECT problem_id FROM problems")
    existing_problems = {row[0] for row in cursor.fetchall()}

    if not verbose:
        print("Processing solutions...", end=" ", flush=True)

    try:
        ds_dict = load_dataset_with_retry(options.config)
    except Exception as e:
        print(f"\n✗ Failed to load dataset after 5 retries: {e}")
        return {"status": "error", "problems_processed": 0}

    max_workers = options.workers if options.workers > 0 else os.cpu_count()

    stats = {
        "problems_with_c": 0,
        "problems_with_cpp": 0,
        "problems_written": 0,
        "solutions_c": 0,
        "solutions_cpp": 0,
        "clusters_total": 0,
        "duplicates_removed": 0,
        "problems_skipped_no_c_family": 0,
        "problems_skipped_below_min": 0,
        "problems_skipped_no_tests": 0,
    }

    problem_batch: list[tuple[str, str]] = []
    solution_batch: list[tuple] = []
    processed = 0

    def flush_batches() -> None:
        if problem_batch:
            cursor.executemany(
                "INSERT OR IGNORE INTO problems VALUES (?, ?)",
                problem_batch,
            )
        if solution_batch:
            cursor.executemany(
                "INSERT OR REPLACE INTO solutions VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",
                solution_batch,
            )
        conn.commit()
        problem_batch.clear()
        solution_batch.clear()

    def handle_result(status: str, result) -> None:
        if status == ProcessStatus.SKIP_NO_C_FAMILY:
            stats["problems_skipped_no_c_family"] += 1
        elif status == ProcessStatus.SKIP_BELOW_MIN:
            stats["problems_skipped_below_min"] += 1
        elif status == ProcessStatus.SKIP_NO_TESTS:
            stats["problems_skipped_no_tests"] += 1
        else:
            problem_row, solution_rows, problem_stats = result
            problem_batch.append(problem_row)
            solution_batch.extend(solution_rows)

            for key in [
                "problems_written",
                "problems_with_c",
                "problems_with_cpp",
                "solutions_c",
                "solutions_cpp",
                "clusters_total",
                "duplicates_removed",
            ]:
                stats[key] += problem_stats[key]

            if len(solution_batch) >= BATCH_SIZE:
                flush_batches()

    with mp.Pool(processes=max_workers) as pool:
        for split_name, split_ds in ds_dict.items():
            if verbose:
                print(f"\nProcessing {split_name} split...")

            args_iter = (
                (row, idx, options.min_solutions, options.cluster_threshold,
                 split_name, existing_problems)
                for idx, row in enumerate(split_ds)
            )

            chunksize = max(1, max_workers * CHUNK_SIZE_MULTIPLIER)

            try:
                for result in pool.imap_unordered(_process_problem_worker, args_iter, chunksize=chunksize):
                    processed += 1
                    if verbose and processed % PROGRESS_INTERVAL == 0:
                        print(f"  Processed {processed}...", file=sys.stderr)

                    status, data = result
                    handle_result(status, data)
            except Exception as e:
                typer.echo(f"Warning: Failed to process problem: {e}", err=True)
                continue

    flush_batches()

    total_solutions = stats["solutions_c"] + stats["solutions_cpp"]
    if not verbose:
        print(f"✓ Download complete [{stats['problems_written']} problems, "
              f"{total_solutions} solutions]")

    return stats


def analysis_phase(conn: sqlite3.Connection, cursor: sqlite3.Cursor,
                   options: Options, verbose: bool) -> dict:
    """Display database statistics. Always runs."""

    cursor.execute("SELECT COUNT(*), COUNT(klee_sym_env) FROM problems")
    total_problems, configured_problems = cursor.fetchone()

    cursor.execute("SELECT COUNT(*) FROM solutions")
    total_solutions = cursor.fetchone()[0]

    cursor.execute(
        "SELECT language, SUM(CASE WHEN correct=1 THEN 1 ELSE 0 END), "
        "COUNT(*) FROM solutions GROUP BY language"
    )
    lang_stats = {row[0]: (row[1], row[2]) for row in cursor.fetchall()}

    section("Database Summary")
    print(f"Total problems:      {total_problems:,}")
    print(f"Total solutions:     {total_solutions:,}")
    if "c" in lang_stats:
        c_correct, c_total = lang_stats["c"]
        print(f"C solutions:         {c_total:,} ({100*c_total/total_solutions:.1f}%)")
    if "cpp" in lang_stats:
        cpp_correct, cpp_total = lang_stats["cpp"]
        print(f"C++ solutions:       {cpp_total:,} ({100*cpp_total/total_solutions:.1f}%)")

    if configured_problems > 0:
        cursor.execute(
            """
            SELECT
                CAST(SUBSTR(klee_sym_env,
                            INSTR(klee_sym_env, '--sym-stdin ') + 12) AS INTEGER) AS size,
                COUNT(*)
            FROM problems
            WHERE klee_sym_env LIKE '--sym-stdin %'
            GROUP BY size
            ORDER BY size
            """
        )
        stdin_counts = dict(cursor.fetchall())

        section("KLEE Stdin Size Distribution")
        print(f"{'Size (bytes)':<15} {'Count':<10} {'Percentage':<12} {'Bar'}")
        print("-" * 60)
        for size, count in stdin_counts.items():
            pct = 100 * count / configured_problems
            bar = "█" * int(pct / 2)
            print(f"{size:<15} {count:<10,} {pct:>6.1f}%      {bar}")

        sizes_sorted = sorted(stdin_counts)
        total_sum = sum(s * c for s, c in stdin_counts.items())
        total_count = sum(stdin_counts.values())
        half, cum, median = total_count / 2, 0, sizes_sorted[-1]
        for s in sizes_sorted:
            cum += stdin_counts[s]
            if cum >= half:
                median = s
                break

        section("Statistics")
        print(f"Min stdin size:      {sizes_sorted[0]:,} bytes")
        print(f"Max stdin size:      {sizes_sorted[-1]:,} bytes")
        print(f"Median stdin size:   {median:,} bytes")
        print(f"Mean stdin size:     {total_sum / total_count:,.0f} bytes")

    return {"status": "ok"}


def main(
    db: Annotated[Path, typer.Option(help="SQLite database path")] = Path("datasets/code-contests-plus.db"),
    config: Annotated[str, typer.Option(help="Dataset config: default, 1x, 2x, 3x, 4x, 5x")] = "5x",
    min_solutions: Annotated[int, typer.Option(help="Skip problems with fewer C/C++ solutions after dedup")] = 2,
    cluster_threshold: Annotated[float, typer.Option(help="Jaccard similarity threshold for near-duplicate clustering")] = 0.85,
    workers: Annotated[int, typer.Option(help="Number of worker processes (0 = CPU count)")] = 0,
    verbose: Annotated[bool, typer.Option(help="Show detailed progress")] = False,
) -> None:
    """Setup Code-Contests-Plus database with solutions and KLEE environments."""

    options = Options(db, config, min_solutions, cluster_threshold, workers, verbose)

    conn = sqlite3.connect(db)
    conn.execute("PRAGMA journal_mode=WAL")
    conn.execute("PRAGMA foreign_keys=ON")
    cursor = conn.cursor()

    try:
        init_phase(conn, cursor, options, verbose)
        download_stats = download_phase(conn, cursor, options, verbose)
        analysis_phase(conn, cursor, options, verbose)
    finally:
        conn.close()


if __name__ == "__main__":
    typer.run(main)
