---
name: replace-array-tracking-with-explicit-termination
description: Use when code uses arrays to track visited states in a sequence/iteration
---

# replace-array-tracking-with-explicit-termination

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses arrays to track visited states in a sequence/iteration

## Transformation

Replace array-based state tracking with explicit checks for known terminal values in the sequence

## Why it helps

Array indexing with symbolic values creates complex memory constraints and path explosion, while explicit value checks create simpler branch conditions that KLEE can explore more efficiently

## Example

Before:

```c
#include <stdio.h>
int a[10000];
int x, n;
int main() {
	scanf("%d", &x);
	while (a[x] < 2) {
		a[x]++;
		if (x % 2)x = 3 * x + 1;
		else x /= 2;
		n++;
	}
	printf("%d\n", n - 2);
}
```

After:

```c
#include<stdio.h>

int main()
{
	int s;
	int i=1;

	scanf("%d",&s);
	// a_n=f(n)= ? n/2 : 3n+1
	while(1)
	{
		if(s==4||s==2||s==1) break;
		s=s%2==0?s/2:s*3+1;
		i++;
	}
	printf("%d\n",i+3);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.8929
- Source problems: train_AtCoder_p03146
- Rule id: `6b5bcb77`
