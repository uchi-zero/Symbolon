// state-killer-finder:
typedef int (*fp)(int);
int run(fp f, int x) { return f(x); }
