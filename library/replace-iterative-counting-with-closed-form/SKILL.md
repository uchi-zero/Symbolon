---
name: replace-iterative-counting-with-closed-form
description: Use when code computes a mathematical quantity (e.g., trailing zeros / factor counts) by iterating over all values up to n with nested loops and modulo checks
---

# replace-iterative-counting-with-closed-form

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code computes a mathematical quantity (e.g., trailing zeros / factor counts) by iterating over all values up to n with nested loops and modulo checks

## Transformation

Replace the nested iteration with the direct closed-form recurrence (e.g., repeatedly divide n by the prime factor and sum the quotients) that produces the same result in O(log n) steps

## Why it helps

KLEE forks paths on each loop iteration and each conditional branch; an O(n log n) loop over symbolic n explodes the path space and creates many modulo constraints, while the O(log n) closed form drastically reduces forks and yields simpler division constraints, letting the solver cover more behaviors within the budget

## Example

Before:

```c
#include<stdio.h>

int main(void){
	int n;
	for(;;){
		scanf("%d",&n);
		if(!n)break;
		int a=0,b;
		for(int i=4;i<=n;i++){
			b=5;
			while(b<=i){
				a+=(!(i%b) && i!=0)? 1 : 0;
				b*=5;
			}
		}
		printf("%d\n",a);
	}
}
```

After:

```c
#include <stdio.h>

int main(void){
	int n;
	int ans;

	while(1){
		ans = 0;
		scanf("%d", &n);
		if(n == 0) break;

		while(1){
			ans = ans + n / 5;
			n = n / 5;
			if(n == 0) break;
		}

		printf("%d\n", ans);
	}

	return 0;
}
```

## Evidence

- Average coverage delta: 0.6139
- Source problems: train_AIZU_p00052
- Rule id: `f3bd5af4`
