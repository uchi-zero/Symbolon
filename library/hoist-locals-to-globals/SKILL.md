---
name: hoist-locals-to-globals
description: Use when intermediate computation results are stored in local variables inside main, embedded directly in expressions, or computed inline within printf arguments
---

# hoist-locals-to-globals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When intermediate computation results are stored in local variables inside main, embedded directly in expressions, or computed inline within printf arguments

## Transformation

Lift scalar variables holding inputs and intermediate results to file-scope globals, and assign each sub-expression (quotient, remainder, ratio) to its own named global before use in output

## Why it helps

Globals reside in fixed symbolic memory regions that KLEE tracks across the whole program, and breaking compound expressions into separately-named assignments creates explicit constraint nodes per sub-expression, letting KLEE branch on and reuse each intermediate value rather than re-deriving it inside nested expression trees

## Example

Before:

```c
#include <stdio.h>

int main(){
int a,b;
double c=0.0;
scanf("%d %d",&a,&b);
c=(double)a/(double)b;
printf("%d %d %f\n",a/b,a%b,c);
return 0;
}
```

After:

```c
#include <stdio.h>

int a,b,d,r;
double f;

int main(){
  scanf("%d %d",&a,&b);
  d=a/b;
  r=a%b;
  f=(double)a/b;
  printf("%d %d %lf\n",d,r,f);
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p02399
- Rule id: `c3cac799`
