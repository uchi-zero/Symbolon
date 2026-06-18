---
name: avoid-early-return-in-loops
description: Use when code uses early return statements inside nested loops to exit upon finding a condition
---

# avoid-early-return-in-loops

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses early return statements inside nested loops to exit upon finding a condition

## Transformation

Replace early returns with state modification (e.g., setting a flag variable) and check the state after loops complete

## Why it helps

Early returns create path termination that prevents KLEE from exploring remaining loop iterations, while state modification allows KLEE to symbolically explore all loop paths and generate more diverse test cases

## Example

Before:

```c
#include<stdio.h>
int main(void) {  int qq, n, i, u;scanf("%d", &n);  for(i = 1;i <= 9;i++) {for(u = 1;u <= 9;u++) { qq = i * u; if(qq == n){puts("Yes"); return 0;}}}puts("No");return 0;}
```

After:

```c
#include <stdio.h>
int n;
int main() {
	scanf("%d", &n);
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 10; j++) {
			if (n == i*j)n = 0;
		}
	}
	if (n)printf("No\n");
	else printf("Yes\n");
}
```

## Evidence

- Average coverage delta: 0.1455
- Source problems: train_AtCoder_p02880
- Rule id: `818c0968`
