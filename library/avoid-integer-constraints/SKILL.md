---
name: avoid-integer-constraints
description: Use when code reads an integer with scanf and performs arithmetic/logical operations on it
---

# avoid-integer-constraints

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code reads an integer with scanf and performs arithmetic/logical operations on it

## Transformation

Read input as a character and perform direct character-based comparisons or arithmetic instead of parsing to integer

## Why it helps

Character input creates simpler constraints (single byte vs multi-byte integer) and avoids complex integer parsing logic, allowing KLEE to explore paths more efficiently

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int x;
	scanf("%d",&x);
	printf("%d\n",!x);
}
```

After:

```c
#include<stdio.h>
int main(){puts(getchar()-48?"0":"1");}
```

## Evidence

- Average coverage delta: 0.1429
- Source problems: train_AtCoder_p02552
- Rule id: `2d0036ef`
