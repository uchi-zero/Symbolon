---
name: early-exit-large-range-shortcut
description: Use when nested loops iterate over a range [a,b] that may be very large, forcing symbolic execution to unroll many iterations to discover that the answer is trivially determined (e.g., when the range exceeds a modulus).
---

# early-exit-large-range-shortcut

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops iterate over a range [a,b] that may be very large, forcing symbolic execution to unroll many iterations to discover that the answer is trivially determined (e.g., when the range exceeds a modulus).

## Transformation

Add an upfront guard that detects the trivial case (e.g., if (b-a) > MOD print 0) and returns immediately, so the expensive nested loop is only entered for small bounded ranges.

## Why it helps

KLEE's path explosion grows with loop iteration counts on symbolic bounds; a guarded shortcut prunes an entire class of inputs into one path with a simple constraint, letting the solver focus on the bounded sub-case and achieve deeper coverage within its budget.

## Example

Before:

```c
#include<stdio.h>

int main()
{
	unsigned long long a,b,i,j,t=2018,m;
	scanf("%ld%ld",&a,&b);
	for(i=a;i<=b-1;i++)
	{
		for(j=i+1;j<=b;j++)
		{
		m=i*j%2019;
		if(m<t)
		t=m;
		if(t==0)
		break;	
		}
		if(t==0)
		break;
	}
	printf("%ld",t);
 }
```

After:

```c
#include<stdio.h>
int main()
{
	long long a,b,m=1000000;
	scanf("%lld%lld",&a,&b);
	if(b-a>2019)printf("0\n");
	else
	{
		for(long long i=a;i<b;i++)
		{
			for(long long j=i+1;j<=b;j++)
			{
				if((i%2019*j%2019)%2019<m)m=(i%2019*j%2019)%2019;
			}
		}
		printf("%lld\n",m);
	}
}
```

## Evidence

- Average coverage delta: 0.3438
- Source problems: train_AtCoder_p02983
- Rule id: `e06f0780`
