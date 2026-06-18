// fork-finder: --include-non-loop
typedef int (*op_t)(int);
int run(op_t fp, int x) { return fp(x); }
