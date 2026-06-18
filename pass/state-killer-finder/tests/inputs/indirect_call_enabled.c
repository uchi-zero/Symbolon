// state-killer-finder: --enable=ExternCallIndirect
typedef int (*fp)(int);
int run(fp f, int x) { return f(x); }
