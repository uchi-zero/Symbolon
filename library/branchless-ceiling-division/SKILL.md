---
name: branchless-ceiling-division
description: Use when code computes ceiling division using a conditional remainder check (e.g., t = a/c; if (a%c != 0) t++;) creating extra symbolic branches
---

# branchless-ceiling-division

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code computes ceiling division using a conditional remainder check (e.g., t = a/c; if (a%c != 0) t++;) creating extra symbolic branches

## Transformation

Replace the conditional increment idiom with the branchless arithmetic form (a + c - 1) / c, and likewise collapse if-based max/min selections into ternary expressions

## Why it helps

Each if-statement on symbolic data forks KLEE's execution state, multiplying path counts and constraint-solving work; arithmetic equivalents keep the computation in a single path so symbolic inputs propagate through fewer, simpler constraints, yielding broader coverage per generated test

## Example

Before:

```c
#include<stdio.h>

int main(){

int L,A,B,C,D;
int t1,t2;
scanf("%d",&L);
scanf("%d",&A);
scanf("%d",&B);
scanf("%d",&C);
scanf("%d",&D);

t1=A/C;if(A%C!=0)t1++;
t2=B/D;if(B%D!=0)t2++;

if(t2>t1)t1=t2;

L=L-t1;
printf("%d\n",L);
return 0;
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int l, a, b, c, d;
  scanf("%d%d%d%d%d", &l, &a, &b, &c, &d);
  a = (a + c - 1) / c, b = (b + d - 1) / d;
  int m = a > b ? a : b;
  printf("%d\n", l - m);
  return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00499
- Rule id: `aed69175`
