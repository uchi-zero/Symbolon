---
name: prefer-ternary-accumulation-over-if-guarded-add
description: Use when the accumulator is updated only inside an if-statement that branches on a comparison between two array elements
---

# prefer-ternary-accumulation-over-if-guarded-add

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the accumulator is updated only inside an if-statement that branches on a comparison between two array elements

## Transformation

Replace the if-guarded addition with an unconditional accumulation using a ternary expression that yields zero in the non-positive case

## Why it helps

An explicit if-branch forces KLEE to fork and create a separate path constraint per loop iteration, causing exponential path blow-up over the array; a ternary expression is lowered to a select instruction that keeps the loop on a single path while still exercising both value cases, so generated tests cover the comparison logic without the path explosion.

## Example

Before:

```c
#include<stdio.h>
int main(){
  int n,v[22],c[22];
  scanf("%d",&n);
  for(int i=0;i<n;i++)scanf("%d",&v[i]);
  for(int i=0;i<n;i++)scanf("%d",&c[i]);
  int ans=0;
  for(int i=0;i<n;i++)if(v[i]-c[i]>0)ans+=v[i]-c[i];
  printf("%d",ans);
}
```

After:

```c
#include <stdio.h>
int V[21], W[21];
int main(){
	int N;
	scanf("%d",&N);
	int sum=0;
	for(int i=0;i<N;i++) scanf("%d",V+i);
	for(int i=0;i<N;i++) scanf("%d",W+i);
	for(int i=0;i<N;i++) sum+=V[i]>W[i]?V[i]-W[i]:0;
	printf("%d",sum);
	
}
```

## Evidence

- Average coverage delta: 0.2167
- Source problems: train_AtCoder_p03060
- Rule id: `0e67574d`
