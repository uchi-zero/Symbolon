// state-killer-finder: --enable=UnimplementedIntrinsic
#include <stdint.h>
uint32_t reverse(uint32_t x) { return __builtin_bitreverse32(x); }
int main(void) { return (int)reverse(0xdeadbeefU); }
