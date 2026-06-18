---
name: avoid-exponential-computation
description: Use when code computes large values through repeated multiplication in loops (e.g., computing powers)
---

# avoid-exponential-computation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code computes large values through repeated multiplication in loops (e.g., computing powers)

## Transformation

Replace multiplication-based computation with equivalent string/output manipulation that produces the same result without computing large intermediate values

## Why it helps

KLEE struggles with symbolic execution of large arithmetic operations and exponential growth values, but can efficiently handle string operations and direct output formatting

## Example

Before:

```c
#include<stdio.h>
int main(){
    int d,n,t=1;
    scanf("%d%d",&d,&n);
    for(int i=0;i<d;i++)t*=100;
    if(n==100)n++;
    printf("%d\n",t*n);
    return 0;
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
- Rule id: `1784bbf4`
