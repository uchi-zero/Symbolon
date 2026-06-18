---
name: wrap-input-in-eof-loop
description: Use when main reads input only once with scanf and exits, limiting symbolic execution to a single input vector per run
---

# wrap-input-in-eof-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When main reads input only once with scanf and exits, limiting symbolic execution to a single input vector per run

## Transformation

Wrap the scanf/processing block in a `while(~scanf(...))` loop so the program repeatedly consumes inputs until EOF, processing multiple test cases per execution

## Why it helps

KLEE can fork on the scanf return value to explore both the EOF-termination path and the continued-processing path, generating test inputs that cover multi-case scenarios and exercising the I/O boundary logic that single-shot programs miss

## Example

Before:

```c
#include<stdio.h>

int main()
{
	int x1, x2, y1, y2;
	scanf("%d%d%d%d", &x1, &y1, &x2, &y2);
	printf("%d %d %d %d\n", x2 - (y2 - y1), y2 + (x2 - x1),
		x1 - (y2 - y1), y1 + (x2 - x1));

	return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	while(~scanf("%d %d %d %d", &x1, &y1, &x2, &y2))
		printf("%d %d %d %d\n", y1-y2+x2, x2-x1+y2, y1-y2+x1, x2-x1+y1);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03265
- Rule id: `9ca66239`
