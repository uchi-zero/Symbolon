---
name: eliminate-redundant-loops-and-io
description: Use when the program contains loops or input reads whose results are not needed for the final output (e.g., reading values into a discarded variable, or iterative computations equivalent to a closed-form expression)
---

# eliminate-redundant-loops-and-io

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program contains loops or input reads whose results are not needed for the final output (e.g., reading values into a discarded variable, or iterative computations equivalent to a closed-form expression)

## Transformation

Replace dead input-consuming loops and iterative reductions with their closed-form arithmetic equivalent, so the program reads only the inputs that affect output and computes the result directly

## Why it helps

KLEE forks on each symbolic loop iteration and each symbolic scanf, causing path explosion and complex constraints; a closed-form expression yields a single path with simple linear constraints, letting the solver generate inputs that cover the underlying logic quickly

## Example

Before:

```c
#include<stdio.h>
int main(){
	int N,Y,b,sum;
	scanf("%d %d",&N,&Y);
	for(int i = 0;i<Y;i++){
		scanf("%d",&b);
	}
	sum = 0;
	for(;;){
		N = N / 2;
		sum = sum + N;
		if(N==1)	break;
	}
	printf("%d\n",sum-Y);
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int a, b;
  scanf("%d%d", &a, &b);
  printf("%d\n", a - b - 1);
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1481
- Source problems: train_AIZU_p01905
- Rule id: `97fee3d3`
