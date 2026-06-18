---
name: avoid-modulo-in-constraints
description: Use when code uses modulo operations (%) in conditional expressions that involve symbolic variables
---

# avoid-modulo-in-constraints

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses modulo operations (%) in conditional expressions that involve symbolic variables

## Transformation

Replace integer arithmetic with modulo checks by using floating-point arithmetic and checking if the result is an integer (e.g., checking if rounded value equals original value)

## Why it helps

Modulo operations create complex non-linear constraints that are difficult for SMT solvers to reason about, while floating-point comparisons with rounding create simpler constraints that KLEE can explore more efficiently

## Example

Before:

```c
#include <stdio.h>
int n;

int main()
{
    scanf("%d",&n);
    //printf("y");
    for (long long i=1;i<=3500;i++)
    for (long long j=i;j<=3500;j++)
    {
        //printf("%lld %lld %lld\n",i,j,(i*j*n)  );
        if ( (3*i*j>n*(i+j) ) && (i*j*n)%( 4*i*j-n*(i+j) )==0  )
        {
            printf("%lld %lld %lld\n",i,j,(i*j*n)/(4*i*j-n*(i+j) ) );
            return 0;
        }

    }
}
```

After:

```c
#include <stdio.h>
#include <math.h>

int main(void){
	double n,i,j,k,ans,ap;
	scanf("%lf",&n);
	ans = 4/n;
	for (i=1;i<5000;i++)
	{
		for (j=1;j<5000;j++)
		{
			ap = ans - 1/i - 1/j;
			if (ap >0)
			{
				ap = 1.0/ap;
				ap = roundl(ap * 10000000) / 10000000;
				if ((int)(ap) == ap) 
				{
					printf("%d %d %d",(int)(i),(int)(j),(int)(ap));
					return 0;
				}
			}
		}
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.5
- Source problems: train_AtCoder_p03583
- Rule id: `a6aff26e`
