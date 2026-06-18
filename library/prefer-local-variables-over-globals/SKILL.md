---
name: prefer-local-variables-over-globals
description: Use when input variables are declared as globals, KLEE tracks them in persistent memory state, increasing the symbolic state size and constraint complexity across the program.
---

# prefer-local-variables-over-globals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input variables are declared as globals, KLEE tracks them in persistent memory state, increasing the symbolic state size and constraint complexity across the program.

## Transformation

Move variable declarations from global scope into main (or the relevant function) as local variables, scoping them to where they are actually used.

## Why it helps

Local variables live on the stack and are scoped narrowly, allowing KLEE to manage symbolic memory more efficiently, reduce aliasing concerns, and produce cleaner path constraints, which improves exploration speed and test-input generality.

## Example

Before:

```c
#include<stdio.h>
int a,b,c,d,e,k;
int main()
{
	scanf("%d%d%d%d%d%d",&a,&b,&c,&d,&e,&k);
	if(e-a>k)puts(":(");
	else puts("Yay!");
}
```

After:

```c
#include <stdio.h>

int main() {
	int a, b, c, d, e, k;
	scanf("%d %d %d %d %d %d", &a, &b, &c, &d, &e, &k);
	if (e - a > k)
		printf(":(\n");
	else
		printf("Yay!\n");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1111
- Source problems: train_AtCoder_p03075
- Rule id: `a77e0d4a`
