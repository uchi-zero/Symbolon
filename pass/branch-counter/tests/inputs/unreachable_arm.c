// branch-counter:
// __builtin_unreachable() puts an `unreachable` instruction on one arm,
// but the predecessor BranchInst still has 2 successors → 2 outcomes.
int positive_only(int x) {
  if (x <= 0)
    __builtin_unreachable();
  return x;
}
