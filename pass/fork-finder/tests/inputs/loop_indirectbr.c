// fork-finder:
// In-loop computed-goto so clang emits indirectbr at loop_depth 1.
// NOTE: clang 16 -O0 does not attach !dbg to the synthesized indirectbr,
// so the golden records this finding under
// summary.unlocated_by_category.Indirect instead of under
// files.loop_indirectbr.c. The loop-depth filter still keeps it because
// LoopInfo derives depth from the basic-block parent, which is in the loop
// body. See indirectbr.c for the non-loop analog.
int dispatch(const unsigned *labels, unsigned n) {
  static void *const jumps[] = {&&L0, &&L1, &&L2};
  int s = 0;
  for (unsigned i = 0; i < n; ++i) {
    goto *jumps[labels[i]];
  L0:
    s += 1;
    continue;
  L1:
    s += 2;
    continue;
  L2:
    s += 3;
    continue;
  }
  return s;
}
