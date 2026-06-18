---
name: check-loop-condition-before-mutation
description: Use when a loop uses an unconditional 'while(1)' with the termination check placed after state mutations and side effects (e.g., increment then test-and-break)
---

# check-loop-condition-before-mutation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop uses an unconditional 'while(1)' with the termination check placed after state mutations and side effects (e.g., increment then test-and-break)

## Transformation

Restructure the loop so the termination condition is tested at the top of the loop body before any state updates, allowing the exit branch to be reached without first executing accumulator updates

## Why it helps

KLEE forks on each branch; placing the exit test first lets the symbolic executor reach and satisfy the termination path in fewer iterations, reducing path explosion and constraint complexity from accumulated arithmetic before the break

## Example

Before:

```c
#include<stdio.h>
int main() {
	int a, b, c;
	int  i = 0, l = 0;
	scanf("%d %d %d", &a, &b, &c);
	while (1) {
		l++;
		if (l% 7 == 0)i += b;
		i += a;
		if (i >= c) {
			printf("%d\n", l); break;
		}
	}
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int a,b,c,x,y;
	scanf("%d %d %d",&a,&b,&c);
	x=0;	y=0;	
	while(1){
		if(x>=c) break;
		x+=a;
		y++;
		if(y%7==0)  x+=b;
	}
	printf("%d\n",y);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1026
- Source problems: train_AIZU_p00575
- Rule id: `69acbbc2`
