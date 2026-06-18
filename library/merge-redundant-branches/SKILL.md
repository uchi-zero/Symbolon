---
name: merge-redundant-branches
description: Use when multiple sequential if/else-if branches produce overlapping or equivalent outputs based on related conditions over the same variables
---

# merge-redundant-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple sequential if/else-if branches produce overlapping or equivalent outputs based on related conditions over the same variables

## Transformation

Collapse the redundant conditional branches into a single combined predicate, using inline expressions (e.g., ternary or max/min) to compute the output instead of separate paths

## Why it helps

KLEE forks a new state at every branch, so redundant branches multiply the path space without adding semantic coverage; merging them reduces path explosion and yields tests that exercise the underlying logic rather than syntactic branch structure, improving cross-solution coverage.

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int m,f,b;
	scanf("%d %d %d",&m,&f,&b);
	if(m>=b){
		printf("0\n");
	}
	else if(f>=b){
		printf("%d\n",b-m);
	}
	else if(m+f>=b){
		printf("%d\n",b-m);
	}
	else printf("NA\n");
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

- Average coverage delta: 0.2617
- Source problems: train_AIZU_p00353
- Rule id: `b9b2ddb7`
