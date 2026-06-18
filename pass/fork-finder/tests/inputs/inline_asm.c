// fork-finder:
// Inline-asm calls must NOT be classified as Call (they are not fork sites).
int rdtsc_low(void) {
  unsigned int lo;
  __asm__ __volatile__("rdtsc" : "=a"(lo)::"edx");
  return (int)lo;
}
