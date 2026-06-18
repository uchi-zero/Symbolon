// fork-finder: --include-non-loop
// Use computed-goto so clang emits indirectbr.
// NOTE: clang 16 -O0 does not attach !dbg to the synthesized indirectbr,
// so the golden records this finding under
// summary.unlocated_by_category.Indirect instead of under files.indirectbr.c.
// If a future clang attaches !dbg, the golden will need regenerating via
// `pass-fixture-runner --update-golden`.
int dispatch(int op) {
  static void *targets[] = {&&L0, &&L1, &&L2};
  if (op < 0 || op > 2)
    return -1;
  goto *targets[op];
L0:
  return 0;
L1:
  return 1;
L2:
  return 2;
}
