---
name: prefer-integer-arithmetic-over-float
description: Use when loop progression depends on multiplying a value by a floating-point constant (e.g., y *= 1.01) with an implicit or no-op cast back to integer
---

# prefer-integer-arithmetic-over-float

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop progression depends on multiplying a value by a floating-point constant (e.g., y *= 1.01) with an implicit or no-op cast back to integer

## Transformation

Replace floating-point scaling with equivalent integer arithmetic (e.g., n = n + n/100) so loop conditions and updates stay in the integer domain

## Why it helps

KLEE's constraint solver handles linear integer arithmetic far more efficiently than floating-point operations; integer formulations let the solver reason about loop bounds and branch conditions precisely, enabling deeper path exploration and reusable test inputs

## Example

Before:

```c
#include<stdio.h>
int main(){
	long long x,y=100,i=0;
	scanf("%lld",&x);
	while(1) {
		y*=1.01;
		(long long)y;
		i++;
		if(y>=x) break;
	}
	printf("%lld",i);
	return 0;
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

- Average coverage delta: 0.2137
- Source problems: train_AtCoder_p02694
- Rule id: `84186b70`
