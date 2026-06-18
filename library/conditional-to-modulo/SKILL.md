---
name: conditional-to-modulo
description: Use when code uses conditional subtraction to wrap values within a range
---

# conditional-to-modulo

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses conditional subtraction to wrap values within a range

## Transformation

Replace if-based range wrapping with modulo operator

## Why it helps

Modulo creates a single linear constraint instead of branching paths, allowing KLEE to explore all values in one execution path rather than splitting into separate paths for overflow and non-overflow cases

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
	int a,b,m;
	while(scanf("%d %d",&a,&b)!=EOF){
		m =(a+b)%24;
		printf("%d\n",m);
	}
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03773
- Rule id: `a365b563`
