---
name: unify-exit-points-via-flag
description: Use when the code uses early exit() calls scattered through guard conditions to terminate on different cases
---

# unify-exit-points-via-flag

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code uses early exit() calls scattered through guard conditions to terminate on different cases

## Transformation

Replace early exit() calls with a single boolean flag set inside an if/else-if chain, and emit output once at the end based on the flag

## Why it helps

KLEE forks paths that terminate via exit() abruptly, losing shared state and making path merging impossible; a single exit point with a flag produces a unified control-flow graph where symbolic constraints accumulate consistently, letting KLEE generate inputs that systematically cover all branches

## Example

Before:

```c
#include <stdio.h>
#include <stdlib.h>
typedef long long ll;
ll a,b,c,k;
int main()
{
    scanf("%lld %lld %lld",&a,&b,&c);
    if(b>=a)
    {
        printf("1\n");
        exit(0);
    }
    if(c>=b)
    {
        printf("-1\n");
        exit(0);
    }
    if((a-b)%(b-c)>=1)
		k=1;
    printf("%lld\n",((a-b)/(b-c)+k)*2+1);
}
```

After:

```c
#include<stdio.h>
int main()
{
	long long  x,y,z;
	scanf("%lld%lld%lld",&x,&y,&z);
	int lose=0;long long anc=1;
	if(y-z<=0&&y>=x)
		lose=1;
	else
		if(y-z>0)
		{
			
			anc=(x-y)/(y-z);
			if(anc*(y-z)+y>=x)  
				anc=anc*2+1;
			else
			{
				anc=(anc+1)*2+1;
			}
			lose=1;
		}
		else
			lose=0;
	if(lose)
		printf("%lld\n",anc);
	else
		printf("-1\n");
		
}
```

## Evidence

- Average coverage delta: 0.115
- Source problems: train_AtCoder_p03505
- Rule id: `e58f90a6`
