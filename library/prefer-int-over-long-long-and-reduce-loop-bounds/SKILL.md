---
name: prefer-int-over-long-long-and-reduce-loop-bounds
description: Use when the program uses wide integer types (long long) and large fixed loop bounds (e.g. 10000 iterations) for a convergence/termination check
---

# prefer-int-over-long-long-and-reduce-loop-bounds

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program uses wide integer types (long long) and large fixed loop bounds (e.g. 10000 iterations) for a convergence/termination check

## Transformation

Use narrower integer types (int) for symbolic inputs and reduce the loop bound to a smaller sufficient constant (e.g. 100), and structure the loop to break on the termination condition rather than embedding early-return prints

## Why it helps

Smaller bit-widths shrink the SMT search space so KLEE solves path constraints faster, and fewer loop iterations means fewer forked states, allowing KLEE to exhaustively explore more reachable branches within its budget

## Example

Before:

```c
#include <stdio.h>

long long a,b,c;

int main()
{
	scanf ("%lld %lld %lld",&a,&b,&c);

	for (int t=0;t<10000;t++){
		if (a % 2 || b % 2 || c % 2){
			printf ("%d\n",t);
			return 0;
		}
		long long p = (b + c) / 2;
		long long q = (c + a) / 2;
		long long r = (a + b) / 2;
		a = p; b = q; c = r;
	}
	puts("-1");

	return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
	int i,n;
	int s[1000];
	int a,b,c,d;
	int x,y,z;
	scanf("%d %d %d",&a,&b,&c);
	for(i=0;i<100;i++)
	{
		if(a%2==0&&b%2==0&&c%2==0)
		{
			x=a/2;
			y=b/2;
			z=c/2;
			a=y+z;
			b=x+z;
			c=x+y;
			
			
			
		}
		
		else break;
		
		
	}
	
	n=i;
	if(n==100) printf("-1");
	else printf("%d",n);
	return 0;
	
	
	
	
}
```

## Evidence

- Average coverage delta: 0.15
- Source problems: train_AtCoder_p03723
- Rule id: `256f3bfc`
