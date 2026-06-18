---
name: prefer-addition-over-subtraction-in-parity-checks
description: Use when checking parity of a combination of symbolic integers using subtraction (e.g., (a - b) % 2) which can produce negative intermediate values and implementation-defined modulo results
---

# prefer-addition-over-subtraction-in-parity-checks

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When checking parity of a combination of symbolic integers using subtraction (e.g., (a - b) % 2) which can produce negative intermediate values and implementation-defined modulo results

## Transformation

Rewrite the parity test using addition instead of subtraction (e.g., (a + b) % 2 == 1), since a+b and a-b have the same parity, and compare explicitly to 1

## Why it helps

KLEE's constraint solver handles unsigned-like nonnegative parity constraints more uniformly; using addition avoids negative-value branches from C's truncated modulo on negatives, reducing path divergence and yielding cleaner test inputs that generalize across solutions

## Example

Before:

```c
#include <stdio.h>

int main(void) {
  int a, b;
  scanf("%d%d", &a, &b);
  if((a - b) % 2) printf("IMPOSSIBLE");
  else printf("%d", (a + b) / 2);
  return 0;
}
```

After:

```c
#include <stdio.h>

int main(){
  int A,B;
  scanf("%d%d",&A,&B);
  if((A+B) % 2 == 1){
    printf("IMPOSSIBLE");
  }else{
    printf("%d",(A+B) / 2 );
  }
}
```

## Evidence

- Average coverage delta: 0.1037
- Source problems: train_AtCoder_p02957
- Rule id: `63cd29d2`
