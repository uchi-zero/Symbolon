---
name: scanf-to-getchar
description: Use when using scanf() to read single characters
---

# scanf-to-getchar

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When using scanf() to read single characters

## Transformation

Replace scanf("%c", &var) with var = getchar() for single character input

## Why it helps

scanf() involves complex format string parsing and internal buffering that creates harder constraints for symbolic execution, while getchar() is a simpler direct character read that generates more tractable path conditions

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
- Rule id: `d8ee5988`
