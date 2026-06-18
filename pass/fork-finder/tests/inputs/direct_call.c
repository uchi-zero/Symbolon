// fork-finder:
int helper(int x) { return x + 1; }
int caller(int x) { return helper(x) + 1; }
