---
name: split-arithmetic-into-branches
description: Use when code uses a single closed-form arithmetic expression (e.g., (s-1)/N+1) to handle boundary cases implicitly within one printf or assignment
---

# split-arithmetic-into-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a single closed-form arithmetic expression (e.g., (s-1)/N+1) to handle boundary cases implicitly within one printf or assignment

## Transformation

Decompose the expression into explicit intermediate variables and add an explicit if-statement that branches on the boundary condition (e.g., s%N==0), assigning different values in each branch

## Why it helps

KLEE explores paths through control-flow branches; collapsing case analysis into branchless arithmetic hides the logical cases as a single path, while explicit if-statements expose multiple paths and generate distinct test inputs covering each boundary case

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
#include<stdio.h>
int main(){
  long long s,t,u;
  scanf("%lld",&s);
  t=s/1000000000+1;
  u=1000000000-s%1000000000;
  if(s%1000000000==0){
    t--;
    u=0;
  }
  printf("0 0 %lld 1000000000 %lld 1",u,t);
}
```

## Evidence

- Average coverage delta: 0.1364
- Source problems: train_AtCoder_p02963
- Rule id: `b6dcefac`
