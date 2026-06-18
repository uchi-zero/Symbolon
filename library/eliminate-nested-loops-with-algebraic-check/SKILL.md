---
name: eliminate-nested-loops-with-algebraic-check
description: Use when nested loops exhaustively search for integer solutions to linear equations
---

# eliminate-nested-loops-with-algebraic-check

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops exhaustively search for integer solutions to linear equations

## Transformation

Replace nested loops with a single loop that iterates over one variable and uses modulo arithmetic to check if the remaining value is divisible by the other coefficient

## Why it helps

Reduces path explosion from O(n*m) to O(n) paths, and replaces complex loop-based constraints with simpler arithmetic constraints that are easier for the SMT solver to reason about

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int n;
	scanf("%d",&n);
	int flag=0;
	for(int i=0;i<=25;i++)
	for(int j=0;j<=15;j++)
	if(4*i+7*j==n) {
		flag=1;
		break;
		
	}
	if(flag) printf("Yes");
	else  printf("No");
	
	
	
}
```

After:

```c
#include<stdio.h>

int main()
{
	int n;
	scanf("%d", &n);

	for (int i = 0; i * 4 <= n; i++)
	{
		if (!((n - i * 4) % 7))
		{
			printf("Yes\n");
			return 0;
		}
	}
	printf("No\n");

	return 0;
}
```

## Evidence

- Average coverage delta: 0.1283
- Source problems: train_AtCoder_p03285
- Rule id: `31c5bf93`
