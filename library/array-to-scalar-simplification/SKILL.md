---
name: array-to-scalar-simplification
description: Use when code uses an array to store a single character/value that is only accessed at one index
---

# array-to-scalar-simplification

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses an array to store a single character/value that is only accessed at one index

## Transformation

Replace the array declaration and indexed access with a simple scalar variable

## Why it helps

Arrays introduce symbolic memory indexing complexity in KLEE, while scalar variables create simpler constraints that are easier to solve and explore

## Example

Before:

```c
#include<stdio.h>
int main()
{
	char a[100];
	scanf("%c",&a[0]); 
	printf("%c",(a[0]+1));
}
```

After:

```c
#include<stdio.h>
int main()
{
	char c1;
	c1=getchar();
	putchar(c1+1);
	putchar('\n');
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1429
- Source problems: train_AtCoder_p02801
- Rule id: `1b05609a`
