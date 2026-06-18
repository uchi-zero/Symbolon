---
name: unroll-array-loops-to-scalars
description: Use when fixed-size array inputs are read and processed via loops with index-dependent comparisons and accumulations
---

# unroll-array-loops-to-scalars

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When fixed-size array inputs are read and processed via loops with index-dependent comparisons and accumulations

## Transformation

Replace the fixed-size array with individual scalar variables, read them in a single scanf, and unroll the loop into explicit straight-line comparisons and additions

## Why it helps

KLEE handles scalar variables more efficiently than symbolic array indexing; unrolling eliminates loop-induced path duplication and symbolic pointer arithmetic, producing simpler constraints that the solver resolves faster and yielding test inputs that exercise the underlying logic more directly

## Example

Before:

```c
#include<stdio.h>
int main(void)
{
	int n[4],e,f,i,a,b;
	for(i=0;i<4;i++)	{
	scanf("%d",&n[i]);
	}
	scanf("%d %d",&e,&f);
	a=100;
	b=0;
	for(i=0;i<4;i++)	{
		b=b+n[i];
		if(n[i]<a)	{
		a=n[i];
		}
	}
	b=b-a;
	if(e>f)	{
	b=b+e;
	}
	else	{
	b=b+f;
	}
	printf("%d\n",b);
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int a,b,c,d,e,f,x,y,z;
	scanf("%d %d %d %d %d %d",&a,&b,&c,&d,&e,&f);
	y=a+b+c+d;
	x=a;
	if(b<x){
			x=b;
	}
	if(c<x){
			x=c;
	}
	if(d<x){
			x=d;
	}
	z=y-x;
	if(e<f) printf("%d\n",f+z);
	else 	printf("%d\n",e+z);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1053
- Source problems: train_AIZU_p00542
- Rule id: `94590533`
