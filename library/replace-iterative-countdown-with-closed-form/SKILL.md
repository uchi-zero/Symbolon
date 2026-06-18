---
name: replace-iterative-countdown-with-closed-form
description: Use when the code uses a loop that repeatedly subtracts fixed amounts from symbolic values until a condition is met, creating many symbolic branch iterations
---

# replace-iterative-countdown-with-closed-form

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code uses a loop that repeatedly subtracts fixed amounts from symbolic values until a condition is met, creating many symbolic branch iterations

## Transformation

Replace the loop with an equivalent closed-form arithmetic expression (e.g., ceiling division (x + step - 1) / step) and combine results with simple max/min operators

## Why it helps

Symbolic loops force KLEE to fork on each iteration's loop condition, exploding the path space and producing complex path constraints; a closed-form expression yields a single path with simple arithmetic constraints that the solver handles quickly, allowing generated tests to cover more behavior in other solutions

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int a[101], temp = 0, ruio = 0;
	for (int i = 1; i <= 5; i++) {
		scanf("%d", &a[i]);
	}
	while (a[2] > 0 || a[3] > 0) {
		a[2]-=a[4];
		a[3] -= a[5];
		ruio++;
	}
	printf("%d\n", a[1]-ruio);
	return 0;
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int l, a, b, c, d;
  scanf("%d%d%d%d%d", &l, &a, &b, &c, &d);
  a = (a + c - 1) / c, b = (b + d - 1) / d;
  int m = a > b ? a : b;
  printf("%d\n", l - m);
  return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00499
- Rule id: `7b55829d`
