---
name: expand-ternary-to-if-else
description: Use when code uses ternary operator (?:) with complex conditions involving logical operators
---

# expand-ternary-to-if-else

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses ternary operator (?:) with complex conditions involving logical operators

## Transformation

Replace ternary expressions with explicit if-else statements, separating each condition

## Why it helps

KLEE can more easily fork execution paths at explicit branch points, and simpler boolean expressions in if statements are easier to solve than compound expressions in ternary operators

## Example

Before:

```c
#include<stdio.h>

int main() { int a, b; scanf("%d %d", &a, &b); printf("%d\n", (a>9|b>9)?-1:a*b);}
```

After:

```c
#include<stdio.h>
int main()
{
	int a,b;
	while(~scanf("%d%d",&a,&b))
	{
		if(a>=10||b>=10)
		printf("-1\n");
		else
		printf("%d\n",a*b);
	}
}
```

## Evidence

- Average coverage delta: 0.6667
- Source problems: train_AtCoder_p02879
- Rule id: `a3e9ccb0`
