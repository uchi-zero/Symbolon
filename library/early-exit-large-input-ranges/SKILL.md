---
name: early-exit-large-input-ranges
description: Use when nested loops iterate over a large symbolic range without an analytic shortcut, forcing SE to unroll many iterations and accumulate complex path constraints
---

# early-exit-large-input-ranges

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops iterate over a large symbolic range without an analytic shortcut, forcing SE to unroll many iterations and accumulate complex path constraints

## Transformation

Add an early branch that detects when the input range guarantees a known answer (e.g., r-l > modulus implies result 0) and returns immediately, restricting the expensive loop to a small bounded range

## Why it helps

KLEE's path explosion is dominated by loop iteration count; pruning the large-range case into a single concrete output reduces fork count and shrinks symbolic loop bounds, letting the solver finish constraints quickly and reach more code paths

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
	int l,r;
	long long i,j;
	scanf("%d%d",&l,&r);
	if (r-l>2019) printf("0\n");
	else {
		int mm=2019;
		for (i=l;i<=r;i++)
		for (j=i+1;j<=r;j++) {
			int tt=(i*j)%2019;
			if (tt<mm) mm=tt;
		}
		printf("%d\n",mm);
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3438
- Source problems: train_AtCoder_p02983
- Rule id: `20cd6e22`
