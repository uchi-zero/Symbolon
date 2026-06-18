---
name: precompute-recursion-to-table
description: Use when the program computes values via deep/branching recursion (e.g., tribonacci-like fn with multiple recursive calls and base-case branches) invoked per symbolic input
---

# precompute-recursion-to-table

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program computes values via deep/branching recursion (e.g., tribonacci-like fn with multiple recursive calls and base-case branches) invoked per symbolic input

## Transformation

Replace the recursive function with an iterative bottom-up DP that fills a fixed-size lookup table once, then index the table by the input value

## Why it helps

Recursion multiplies path conditions: each call site adds branch points (base cases, sign checks) that KLEE must fork on, and the call tree explodes exponentially with input size. A precomputed array reduces the per-query computation to a single concrete array access, collapsing the symbolic path space and letting KLEE's constraint solver focus on the input-validation logic instead of unrolling recursion.

## Example

Before:

```c
#include<stdio.h>
int n;

int fn(int n)
{
    if(n==0)return 1;
    if(n<0)return 0;
    return fn(n-1)+fn(n-2)+fn(n-3);
}

int main()
{
    while(1)
    {
        scanf("%d",&n);
        if(n==0)break;
        printf("%d\n",(((fn(n)+10-1)/10)+365-1)/365);
    }
    return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
    int i=2,n,dp[31];
    dp[0]=dp[1]=1;
    dp[2]=2;
    while(++i<31)
        dp[i]=dp[i-1]+dp[i-2]+dp[i-3];
    while(scanf("%d",&n),n)
        printf("%d\n",dp[n]/(10*365)+1);
}
```

## Evidence

- Average coverage delta: 0.1043
- Source problems: train_AIZU_p00168
- Rule id: `c98b13e8`
