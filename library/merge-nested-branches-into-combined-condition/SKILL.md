---
name: merge-nested-branches-into-combined-condition
description: Use when code uses nested if/else branches with separate conditions on the same variables, creating multiple independent path constraints that fragment the symbolic execution tree
---

# merge-nested-branches-into-combined-condition

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses nested if/else branches with separate conditions on the same variables, creating multiple independent path constraints that fragment the symbolic execution tree

## Transformation

Combine the nested conditions into a single boolean expression at the top level, and replace inner branches with conditional expressions (ternary) for value selection rather than control flow

## Why it helps

Fewer branch points reduce the number of distinct paths KLEE must explore; ternary expressions become value-level constraints solvable in one path instead of forking, allowing broader coverage with fewer queries

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int m,f,b;
	int ans=0;

	scanf("%d",&m);
	scanf("%d",&f);
	scanf("%d",&b);

	if(m>=b){
		printf("0\n");
	}else{
		ans=b-m;
		if(f>=ans){
			printf("%d\n",ans);
		}else{
			printf("NA\n");
		}
	}
	return 0;
}
```

After:

```c
#include <stdio.h>


int main(void) {
  int m, f, b;
  scanf("%d%d%d",&m, &f, &b);
  if(m + f >= b)printf("%d\n", b - m > 0 ? b - m : 0);
  else printf("NA\n");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1796
- Source problems: train_AIZU_p00353
- Rule id: `4edb4a83`
