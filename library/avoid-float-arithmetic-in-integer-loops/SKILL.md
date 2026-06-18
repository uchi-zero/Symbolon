---
name: avoid-float-arithmetic-in-integer-loops
description: Use when integer values are updated using floating-point multiplication (e.g., y = y * 1.01) inside a loop condition that KLEE must symbolically reason about
---

# avoid-float-arithmetic-in-integer-loops

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When integer values are updated using floating-point multiplication (e.g., y = y * 1.01) inside a loop condition that KLEE must symbolically reason about

## Transformation

Replace the floating-point scaling with equivalent integer arithmetic (e.g., n = n + n/100), keeping all variables and operations in integer domain

## Why it helps

KLEE's constraint solver (STP/Z3) handles bitvector/integer constraints far more efficiently than floating-point ones; floating-point operations are often concretized or poorly modeled, blocking path exploration and reducing achievable branch coverage

## Example

Before:

```c
#include<stdio.h>
int main(){
	long long x,y=100;
	scanf("%lld",&x);
	for(int i=1;;++i){
		y=y*1.01;
		if(y>=x){
			printf("%d\n",i);
			break;
		}
	}
}
```

After:

```c
#include <stdio.h>
long long n, m, i, j, k;
int main(){
	scanf("%lld", &m);
	n = 100;
	while(n < m){
		n = n + n/100;
		k++;
	}
	printf("%lld\n", k);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3137
- Source problems: train_AtCoder_p02694
- Rule id: `cc80f283`
