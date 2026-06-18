---
name: avoid-loop-based-search
description: Use when code uses a loop to search for a value that satisfies an equation or constraint
---

# avoid-loop-based-search

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a loop to search for a value that satisfies an equation or constraint

## Transformation

Replace the loop with a direct mathematical calculation to find the solution, using inverse operations or algebraic manipulation

## Why it helps

Loops create many path constraints that KLEE must explore sequentially, while direct calculations produce simpler constraints that can be solved more efficiently by the constraint solver

## Example

Before:

```c
#include <stdio.h>
int n;
int main() {
	scanf("%d", &n);
	for (int i = 1; i <= n; i++) {
		if (i * 108 / 100 == n) {
			printf("%d\n", i);
			return 0;
		}
	}
	printf(":(\n");
}
```

After:

```c
#include <stdio.h>
#include <math.h>

int main(int argc, char const *argv[])
{
  int n,m;

  scanf("%d",&n);
  m=ceil(n/1.08);
  if(n==(int)(m*1.08)) printf("%d",m);
  else printf(":(");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p02842
- Rule id: `ef425e7f`
