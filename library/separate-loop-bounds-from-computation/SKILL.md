---
name: separate-loop-bounds-from-computation
description: Use when loop bounds and computed values are interdependent within the same loop condition (e.g., j=j*i where j is both the loop variable and the computed value)
---

# separate-loop-bounds-from-computation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop bounds and computed values are interdependent within the same loop condition (e.g., j=j*i where j is both the loop variable and the computed value)

## Transformation

Separate the loop counter from the computed value by using an independent counter variable and computing the value inside the loop body using a function call or separate expression

## Why it helps

KLEE can more easily reason about loop bounds when they are simple counters rather than complex expressions, and function calls like pow() create cleaner symbolic expressions than iterative multiplication

## Example

Before:

```c
#include<stdio.h>
int main(){
int n,i,j,ans=1;
scanf("%d",&n);
for(i=1;i<40;i++){
j=i;
if(i!=1){
while(j<=n){
j=j*i;
if(j<=n&&j>ans)
ans=j;
}
}
}
printf("%d\n",ans);
return 0;
}
```

After:

```c
#include <stdio.h>
#include<math.h>
int i;
int j;
int k=2;
int a;
int max=1;
int main(){
scanf("%d",&i);
for(j=2;j<i;j++){
k=2;
while(1){
a=pow(j,k);
if(a>i)break;
if(max<a)max=a;
k++;
}
}
printf("%d",max);
}
```

## Evidence

- Average coverage delta: 0.537
- Source problems: train_AtCoder_p03352
- Rule id: `54a5cfd1`
