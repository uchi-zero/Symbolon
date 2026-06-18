---
name: factor-compound-arithmetic-into-named-intermediates
description: Use when the code computes complex compound arithmetic expressions inline (e.g., repeated subexpressions like (s-1)/1000000000+1 used multiple times) directly inside output or branching statements
---

# factor-compound-arithmetic-into-named-intermediates

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code computes complex compound arithmetic expressions inline (e.g., repeated subexpressions like (s-1)/1000000000+1 used multiple times) directly inside output or branching statements

## Transformation

Decompose compound expressions into separate intermediate variables assigned step-by-step, so each derived quantity (e.g., a = ceil(n/m); b = ceil(n/a)) is computed once and named before use

## Why it helps

KLEE builds symbolic constraints per assignment; reusing named intermediates yields simpler, shared subexpressions in the constraint store, letting the solver cache and prune paths faster, while inline duplicated arithmetic produces larger redundant constraints that slow STP/Z3 and reduce replay coverage

## Example

Before:

```c
#include <stdio.h>
long long s;
int main(void){
    scanf("%lld",&s);
    printf("%lld %lld %lld %lld %lld %lld\n",0,0,1,1000000000,(s-1)/1000000000+1,((s-1)/1000000000+1)*1000000000-s);
}
```

After:

```c
#include <stdio.h>
#include <math.h>
typedef long long ll;

int main(void) {
  ll i, j, k, n, m = 1e9, a, b;
  printf("0 0 ");
  scanf("%lld", &n);
  a = (n + m - 1) / m;
  b = (n + a - 1) / a;
  printf("%lld %lld 1 %lld", a, a * b - n, b);
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1364
- Source problems: train_AtCoder_p02963
- Rule id: `b09c5434`
