---
name: avoid-string-ops-for-single-char
description: Use when using string operations (scanf with %s, string arrays) to read/process single characters
---

# avoid-string-ops-for-single-char

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When using string operations (scanf with %s, string arrays) to read/process single characters

## Transformation

Replace string operations with character operations (getchar/putchar instead of scanf/printf with %s)

## Why it helps

String operations introduce symbolic memory modeling complexity and null-terminator constraints, while character operations create simpler symbolic expressions that KLEE can explore more efficiently

## Example

Before:

```c
#include<stdio.h>
char alp[2];
int main(void){
  scanf("%s",alp);
  alp[0]++;
  printf("%s",alp);
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
- Rule id: `59cee9c5`
