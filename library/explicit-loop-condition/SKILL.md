---
name: explicit-loop-condition
description: Use when the loop uses 'while(1)' with an internal break controlled by a derived condition, hiding the termination predicate from the symbolic executor
---

# explicit-loop-condition

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the loop uses 'while(1)' with an internal break controlled by a derived condition, hiding the termination predicate from the symbolic executor

## Transformation

Refactor the loop to place the termination predicate directly in the loop header (e.g., 'while (C > coin)') and remove the internal break

## Why it helps

KLEE forks paths at branch conditions; an explicit loop-header predicate exposes the termination constraint to the constraint solver early, enabling cleaner path exploration and avoiding redundant branch states from a separate break check

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
	int A = 0, B = 0, C = 0, RESULT = 0, day = 0, coin = 0;

	scanf("%d%d%d", &A, &B, &C);

	while (C > coin)
	{
		RESULT++;
		day++;
		coin += A;

		if (day == 7)
		{
			day = 0;
			coin += B;
		}
	}

	printf("%d\n", RESULT);

	return 0;
}
```

## Evidence

- Average coverage delta: 0.1961
- Source problems: train_AIZU_p00575
- Rule id: `2e422821`
