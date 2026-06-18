---
name: array-to-scalar-chars
description: Use when code reads a fixed number of characters into an array and only accesses individual elements
---

# array-to-scalar-chars

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code reads a fixed number of characters into an array and only accesses individual elements

## Transformation

Replace the character array with individual character variables and use getchar() instead of scanf()

## Why it helps

KLEE handles scalar variables more efficiently than arrays, reducing symbolic memory complexity and constraint solving overhead

## Example

Before:

```c
#include <stdio.h>

int main()
{
	char S[50];
		scanf("%s", &S);
	if( S[0] == S[1] && S[1] == S[2]) {
		printf("No");
	}
	else {
		printf("Yes");
	}
	
	return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
	register char a,b,c;
	a=getchar();b=getchar();c=getchar();
	if(a==b&&b==c)puts("No");
	else puts("Yes");
}
```

## Evidence

- Average coverage delta: 0.2358
- Source problems: train_AtCoder_p02753
- Rule id: `031368b7`
