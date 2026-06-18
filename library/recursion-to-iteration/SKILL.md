---
name: recursion-to-iteration
description: Use when recursive functions are used for computations that can be expressed iteratively
---

# recursion-to-iteration

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When recursive functions are used for computations that can be expressed iteratively

## Transformation

Replace recursive function calls with equivalent iterative loops, moving the computation directly into main() or keeping it in a separate iterative function

## Why it helps

Recursive calls create complex path constraints and deep call stacks that make symbolic execution harder, while iterative loops have simpler control flow and constraints that KLEE can explore more efficiently

## Example

Before:

```c
#include <stdio.h>

long long fac(int n) {
	return n == 0 ? 1 : fac(n - 1) * n % 1000000007;
}

int main() {
	int n;

	scanf("%d", &n);
	printf("%lld\n", fac(n));
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
- Rule id: `bdf71df8`
