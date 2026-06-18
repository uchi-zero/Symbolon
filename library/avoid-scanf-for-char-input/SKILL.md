---
name: avoid-scanf-for-char-input
description: Use when using scanf() to read a single character into an integer variable
---

# avoid-scanf-for-char-input

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When using scanf() to read a single character into an integer variable

## Transformation

Replace scanf("%c", &int_var) with getchar() and use char type instead of int for character operations

## Why it helps

scanf with %c format on int variables creates complex symbolic constraints due to type mismatches and format string parsing overhead, while getchar() with char type produces simpler constraints that KLEE can solve more efficiently

## Example

Before:

```c
#include<stdio.h> 
int main() {
	int a;
	int b;
	scanf("%c",&a);
	b=++a;
	printf("%c",b);
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
- Rule id: `645ca7c5`
