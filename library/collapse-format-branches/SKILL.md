---
name: collapse-format-branches
description: Use when code branches solely to choose between printf format strings (e.g., zero-padding via if/else)
---

# collapse-format-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code branches solely to choose between printf format strings (e.g., zero-padding via if/else)

## Transformation

Replace the conditional formatting branches with a single printf using format specifiers like %02d that handle the formatting intrinsically

## Why it helps

Eliminating data-dependent branches reduces the number of symbolic paths KLEE must explore, so test inputs cover behavior more densely without redundant forks on purely cosmetic output logic

## Example

Before:

```c
#include<stdio.h>

int main(void){
	int i,kai;

	while(scanf("%d",&i)!=EOF){
		i%=39;
		if(i==0)i=39;
		if(i>9)
			printf("3C%d\n",i);
		else
			printf("3C0%d\n",i);
	}
	return 0;
}
```

After:

```c
#include <stdio.h>

int main(void){
		
	int a;
		
	while(1){
		if(scanf("%d", &a) == EOF )
				break;
			
		a = a % 39;
		if(a == 0)	a = 39;
		printf("3C%02d\n", a);
	}
		
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00148
- Rule id: `f2d99d8a`
