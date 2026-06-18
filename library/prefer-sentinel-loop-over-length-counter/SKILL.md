---
name: prefer-sentinel-loop-over-length-counter
description: Use when a loop iterates over a string using a separately-read length variable as the bound
---

# prefer-sentinel-loop-over-length-counter

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop iterates over a string using a separately-read length variable as the bound

## Transformation

Replace the length-bounded loop with a sentinel-based loop that iterates until the string's null terminator

## Why it helps

Tying the loop bound to the concrete string length (via strlen-like null-terminator checks) removes a symbolic dependency between two independent inputs, reducing path explosion from inconsistent length/string combinations and letting KLEE solve simpler per-iteration constraints

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int a,b;
	char c[12];
	scanf("%d",&a);
	scanf("%s",c);
	scanf("%d",&b);
	for(int i=0;i<a;i++)
	{
		if(c[i]!=c[b-1])
		c[i]='*';
	}
	printf("%s",c);
}
```

After:

```c
#include <stdio.h>
int main()
{
	int N, K;
	char S[11];
	scanf("%d%s%d", &N, S, &K);
	int i;
	for (i = 0; S[i] != '\0'; i++) {
		if (S[i] != S[K-1])S[i] = '*';
	}
	printf("%s\n", S);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.4614
- Source problems: train_AtCoder_p03068
- Rule id: `36e6e976`
