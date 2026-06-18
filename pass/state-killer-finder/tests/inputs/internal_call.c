// state-killer-finder: --enable=all
static int helper(int x) { return x + 1; }
int caller(int x) { return helper(x); }
