---
name: widen-accumulator-types-to-prevent-overflow
description: Use when loop accumulators repeatedly multiplied by constants are stored in narrow int types that can overflow for symbolic loop bounds, causing path explosion or undefined-behavior branches in KLEE
---

# widen-accumulator-types-to-prevent-overflow

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop accumulators repeatedly multiplied by constants are stored in narrow int types that can overflow for symbolic loop bounds, causing path explosion or undefined-behavior branches in KLEE

## Transformation

Promote accumulator variables (e.g., t, s) from int to long long int and adjust the printf format specifier accordingly, as done in Solution B with sum and sub

## Why it helps

Wider integer types delay or eliminate overflow, reducing the number of feasible symbolic paths KLEE must explore around overflow boundaries and simplifying constraints, so the generated test inputs cover more meaningful behavior rather than overflow corner cases

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int num;
	scanf("%d", &num);
	int t = 1, s = 1;
	for (int i = 0; i < num; i++)
	{
		int z;
		scanf("%d", &z);
		if (z % 2 == 0)t *= 2;
		s *= 3;
	}
	printf("%d\n", s - t);
}
```

After:

```c
#include<stdio.h>
#include<math.h>
int main()
{
	int n,a,i;
	long long int sum,sub;
	sum=1;
	sub=1;
	scanf("%d",&n);
	for (i=1;i<=n;i++)
	{
		sum=sum*3;
		scanf("%d",&a);
		if (a%2==0)
			sub=sub*2;
	}
	printf("%lld\n",sum-sub);
	return 0;

	


}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03568
- Rule id: `d70c2d64`
