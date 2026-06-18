---
name: optimize-prime-checking-loop
description: Use when checking primality by iterating from n/2 down to 2 and testing all divisors
---

# optimize-prime-checking-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When checking primality by iterating from n/2 down to 2 and testing all divisors

## Transformation

Replace with iteration from 3 up to sqrt(n) testing only odd divisors, and handle even numbers separately

## Why it helps

Reduces loop iterations from O(n) to O(sqrt(n)) and skips even divisors, creating simpler constraints for KLEE to solve and fewer symbolic execution states to explore

## Example

Before:

```c
#include <stdio.h>

int prime(int n)
{
	int i;
	for(i=n/2; i>=2; i--){
		if(n%i==0){
			return 1;
		}
	}
	return 0;
}

int main(void)
{
	int n,i,j,x,y,z=0;
	scanf("%d",&n);
	for(i=0; i<n; i++){
		for(j=2; z<n; j=j+2){
			x=j*5+1;	y=prime(x);
			if(y==0){
				z=z+1;
				printf("%d",x);
				if(z<n){
					printf(" ");
				}
			}
		}
	}
	printf("\n");
	return 0;
}
```

After:

```c
#include <stdio.h>
#include <math.h>




int main()
{
	int i, j, k,n,l;
	scanf("%d", &n);
	printf("2 ");
	l = 1;
	for (i = 3;;i += 2)
	{
		k = 0;
		for (j = 3;j <= sqrt(i);j += 2)
		{
			if (i%j == 0)
			{
				k = 1;
				break;
			}
		}

		if (k == 0&&i%5==1) { printf("%d ", i); ++l;if (l == n) break; }
	}

	printf("\n");

	return 0;
}
```

## Evidence

- Average coverage delta: 0.25
- Source problems: train_AtCoder_p03362
- Rule id: `591a58bd`
