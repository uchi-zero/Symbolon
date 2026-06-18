---
name: loop-to-closed-form
description: Use when iterative loops compute mathematical sequences or powers that have closed-form solutions
---

# loop-to-closed-form

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When iterative loops compute mathematical sequences or powers that have closed-form solutions

## Transformation

Replace the iterative computation with its mathematical closed-form expression using standard library functions

## Why it helps

Eliminates loop iterations and conditional branches, reducing path explosion and allowing KLEE to directly reason about the mathematical relationship rather than exploring multiple loop paths

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int n,i,x;
	scanf("%d",&n);
	x=1;
	while(1){
		x*=2;
		if(n<x){
			break;
		}
	}
	printf("%d\n",x/2);
	return 0;
}
```

After:

```c
#include<stdio.h>
#include<math.h>

int n;

int main(void){
	scanf("%d",&n);
	printf("%d\n",1<<((int)log2(n)));
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03644
- Rule id: `68e697c5`
