---
name: avoid-reusing-input-variables
description: Use when input variables are modified and reused for computation instead of using separate variables
---

# avoid-reusing-input-variables

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input variables are modified and reused for computation instead of using separate variables

## Transformation

Introduce a new variable to store the result of computations instead of modifying the original input variables

## Why it helps

KLEE can better track constraints when input variables remain unchanged, as modifying inputs creates more complex symbolic expressions that are harder to reason about and may limit path exploration

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int a,b;
	scanf("%d %d",&a,&b);
	a+=b;
	if(a>23)a-=24;
	printf("%d\n",a);
	return 0;
}
```

After:

```c
#include<stdio.h>

int main(){
	int a,b,c;
	scanf("%d %d",&a,&b);
	c=a+b;
	if(c>=24) c-=24;
	printf("%d",c);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03773
- Rule id: `a70a4f99`
