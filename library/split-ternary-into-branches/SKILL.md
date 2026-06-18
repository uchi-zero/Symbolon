---
name: split-ternary-into-branches
description: Use when the program collapses distinct logical cases into a single nested ternary expression inside a printf/return, producing one straight-line path with combined constraints
---

# split-ternary-into-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program collapses distinct logical cases into a single nested ternary expression inside a printf/return, producing one straight-line path with combined constraints

## Transformation

Refactor the ternary into explicit if/else statements with separate output computations, and split a compound arithmetic formula into additive subterms that mirror the underlying case structure

## Why it helps

KLEE forks on control-flow branches, not on operands of ternary operators; converting ?: into if/else creates distinct paths whose path constraints can be solved independently, yielding test inputs that exercise each case and improving coverage when replayed

## Example

Before:

```c
#include <stdio.h>

int main()
{
long long a,b; scanf ("%lld %lld",&a,&b); printf("%lld\n",a==1||b==1?1:(a*b+1)/2);
}
```

After:

```c
#include <stdio.h>

int main() {
long w=1;
long h=1;
scanf("%ld %ld",&w,&h);
if (w==1 || h==1)
printf("1");
else
printf("%ld",(w/2+w%2)*(h/2+h%2)+(w/2)*(h/2));
return 0;
}
```

## Evidence

- Average coverage delta: 0.1333
- Source problems: train_AtCoder_p02742
- Rule id: `7314b42d`
