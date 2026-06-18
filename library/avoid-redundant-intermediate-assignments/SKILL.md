---
name: avoid-redundant-intermediate-assignments
description: Use when code introduces an extra variable mutation (e.g., K-=1) before using the value as an array index, creating additional symbolic state updates
---

# avoid-redundant-intermediate-assignments

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code introduces an extra variable mutation (e.g., K-=1) before using the value as an array index, creating additional symbolic state updates

## Transformation

Inline the arithmetic directly into the index expression (e.g., s[k-1] += 32) instead of pre-decrementing the variable on a separate statement

## Why it helps

Inlining reduces the number of symbolic store operations and intermediate constraints KLEE must track, simplifying the path conditions and enabling faster constraint solving and broader exploration

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int N,K;
	char s[55];
	scanf("%d %d",&N,&K);
	scanf("%s",&s);
	K-=1;
	s[K]+=32;
	printf("%s\n",s);
	return 0;
}
```

After:

```c
#include <stdio.h>
int main (){
	int n,k;
	char s[60] ;
	
	scanf("%d %d",&n ,&k);
	scanf("%s",&s);
	s[k-1] += 32;
	printf("%s",s);
	
	
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03041
- Rule id: `5dc0f752`
