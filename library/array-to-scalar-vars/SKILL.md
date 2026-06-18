---
name: array-to-scalar-vars
description: Use when code reads fixed-size input into an array and only accesses individual elements
---

# array-to-scalar-vars

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code reads fixed-size input into an array and only accesses individual elements

## Transformation

Replace array with individual scalar variables, reading each input element into a separate variable

## Why it helps

KLEE handles scalar variables more efficiently than arrays, reducing symbolic memory complexity and constraint solving overhead

## Example

Before:

```c
#include <stdio.h>
char a[5];
int main(){
	scanf("%s",a);if(a[0]==a[1]&&a[1]==a[2]&&a[0]==a[2]) puts("No");else puts("Yes");return 0;
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
- Rule id: `9e8a2092`
