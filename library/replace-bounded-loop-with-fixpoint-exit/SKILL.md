---
name: replace-bounded-loop-with-fixpoint-exit
description: Use when code uses an arbitrary large iteration cap (e.g., for t<10000) to bound a loop that should instead terminate on a semantic fixpoint or invariant condition
---

# replace-bounded-loop-with-fixpoint-exit

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses an arbitrary large iteration cap (e.g., for t<10000) to bound a loop that should instead terminate on a semantic fixpoint or invariant condition

## Transformation

Replace the bounded counter loop with an infinite loop that exits via a precise termination predicate (e.g., detecting the all-equal fixpoint that causes nontermination), and increment a counter inside

## Why it helps

KLEE forks and explores each loop iteration symbolically; a 10000-iteration cap explodes the path space and stalls the solver, while an explicit fixpoint check lets KLEE terminate paths early and reach diverse program states with fewer constraints

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
  int a,b,c;
  scanf("%d%d%d",&a,&b,&c);
  int r=0;
  for(;;)
  {
    if(a%2+b%2+c%2!=0)break;
    if(a==b&b==c)
    {
      printf("-1\n");
      return 0;
    }
    int s=a/2+b/2+c/2;
    a=s-a/2,b=s-b/2,c=s-c/2;
    r++;
  }
  printf("%d\n",r);
}
```

## Evidence

- Average coverage delta: 0.15
- Source problems: train_AtCoder_p03723
- Rule id: `50a353d1`
