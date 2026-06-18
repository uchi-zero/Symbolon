---
name: avoid-loop-dependent-multiplication
description: Use when a loop performs repeated multiplication where the loop bound depends on symbolic input
---

# avoid-loop-dependent-multiplication

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop performs repeated multiplication where the loop bound depends on symbolic input

## Transformation

Replace multiplication loops with equivalent operations that avoid iterative computation, such as printing zeros for powers of 10 or using closed-form expressions

## Why it helps

KLEE must explore exponentially many paths through multiplication loops with symbolic bounds, while direct output or closed-form solutions create simpler constraints that are easier to solve

## Example

Before:

```c
#include <stdio.h>

int main()
{
    int d,n;
    scanf("%d%d",&d,&n);
    if(n==100) n++;
    for(int i=0;i<d;i++)
    {
        n*=100;
    }
    printf("%d\n",n);
}
```

After:

```c
#include <stdio.h>
int main(){
    int a,b;
    scanf("%d%d",&a,&b);
    if(b==100)b++;
    printf("%d",b);
    for(int i=0;i<a*2;i++)printf("0");
    printf("\n");
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03324
- Rule id: `899d65c8`
