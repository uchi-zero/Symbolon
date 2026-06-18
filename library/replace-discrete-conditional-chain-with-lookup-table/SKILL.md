---
name: replace-discrete-conditional-chain-with-lookup-table
description: Use when code maps a small set of discrete input values to constants via a chain of if/else-if comparisons
---

# replace-discrete-conditional-chain-with-lookup-table

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code maps a small set of discrete input values to constants via a chain of if/else-if comparisons

## Transformation

Replace the conditional chain with a constant array indexed by the input value, performing a single array access instead of branching

## Why it helps

KLEE forks execution at every branch, creating exponential path growth for if/else chains; an array indexed access is a single straight-line operation that avoids path explosion and lets the symbolic input flow directly into a memory dereference, producing simpler constraints and broader downstream coverage

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int a,b,c,d,t,i;
	for(i=1;i<=4;i++){
		scanf("%d %d",&a,&b);
		if(a==1){
			a=6000;
		}
		else if(a==2){
			a=4000;
		}
		else if(a==3){
			a=3000;
		}
		else if(a==4){
			a=2000;
		}
		c=a*b;
		printf("%d\n",c);
	}
	
		
	return 0;
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int i, j, k, a, b, p[5] = {0, 6000, 4000, 3000, 2000};
  for(i = 0; i < 4; ++i) {
    scanf("%d%d", &a, &b);
    printf("%d\n", p[a] * b);
  }
  return 0;
}
```

## Evidence

- Average coverage delta: 0.3684
- Source problems: train_AIZU_p00272
- Rule id: `560d390a`
