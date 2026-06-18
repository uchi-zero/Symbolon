---
name: prefer-decrementing-while-over-for-counter
description: Use when a fixed-iteration loop uses a for-statement with an incrementing counter compared against a constant upper bound (e.g., for(i=0;i<N;i++))
---

# prefer-decrementing-while-over-for-counter

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a fixed-iteration loop uses a for-statement with an incrementing counter compared against a constant upper bound (e.g., for(i=0;i<N;i++))

## Transformation

Rewrite as a while-loop initialized to N that decrements toward zero and tests inequality with zero (e.g., i=N; while(i!=0){...; i--;})

## Why it helps

KLEE's symbolic state and constraint solver handle comparisons against zero more efficiently than against arbitrary constants, and a single decrement-and-test produces simpler path conditions per iteration, reducing solver load and enabling deeper exploration within the same budget.

## Example

Before:

```c
#define scanf_s scanf
#include<stdio.h>
int main(void) {
	int a, b, i;
	for (i = 0; i < 7; i++) {
		scanf_s("%d %d", &a, &b);
		printf("%d\n", a - b);
	}
	
	return(0);
}
```

After:

```c
#include<stdio.h>

int main(void){

	int i=7,a,b;
	while(i!=0){
		scanf("%d %d",&a,&b);
		printf("%d\n",a-b);
		i--;
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00271
- Rule id: `3db21883`
