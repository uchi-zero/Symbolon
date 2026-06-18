---
name: reuse-input-variable
description: Use when code uses a separate accumulator variable for iterative computations instead of reusing the input variable
---

# reuse-input-variable

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a separate accumulator variable for iterative computations instead of reusing the input variable

## Transformation

Reuse the input variable as the accumulator in loops, eliminating the need for a separate variable and its initialization

## Why it helps

Reduces the number of symbolic variables KLEE must track, simplifying the symbolic state and making constraint solving more efficient

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	long long int n,i,w=1,c=0;
	scanf("%lld",&n);
	for(i=1;i<=n;i++){
		w=w*i;
		w=w%1000000007;
	}
	printf("%lld\n",w);
	return 0;
}
```

After:

```c
#include <stdio.h>

int main()
{
	long long a,i;
	const long long MOD = 1000000007;
	scanf("%lld",&a);
	for(i=a-1;i>0;i--)
	{
		a = a*i%MOD;
	}
	printf("%lld\n",a);
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03796
- Rule id: `a4658431`
