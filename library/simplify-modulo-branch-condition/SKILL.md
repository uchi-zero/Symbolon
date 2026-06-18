---
name: simplify-modulo-branch-condition
description: Use when a branch condition compares a modulo result to zero using an explicit equality (e.g., x%y==0), adding an extra symbolic constraint layer
---

# simplify-modulo-branch-condition

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a branch condition compares a modulo result to zero using an explicit equality (e.g., x%y==0), adding an extra symbolic constraint layer

## Transformation

Rewrite the condition to use the modulo result directly as a boolean (e.g., if(x%y) ... else ...), inverting the branches accordingly

## Why it helps

KLEE handles direct nonzero-truthiness constraints more efficiently than explicit ==0 comparisons, reducing solver overhead and helping the symbolic executor enumerate both branches with simpler path conditions

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int x,y;
	scanf("%d%d",&x,&y);
	if(x%y==0)
		printf("-1\n");
	else
		printf("%d\n",x);
  return 0;
}
```

After:

```c
#include<stdio.h>

int x,y;

int main(void){
	scanf("%d%d",&x,&y);
	if(x%y)
		printf("%d",x);
	else
		printf("-1");
	return putchar('\n'),0;
}
```

## Evidence

- Average coverage delta: 0.7407
- Source problems: train_AtCoder_p03437
- Rule id: `cbf3a69b`
