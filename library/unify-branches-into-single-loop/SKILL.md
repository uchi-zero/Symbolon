---
name: unify-branches-into-single-loop
description: Use when the code splits into divergent branches based on input-magnitude heuristics, each running a similar loop with different bounds and update rules
---

# unify-branches-into-single-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code splits into divergent branches based on input-magnitude heuristics, each running a similar loop with different bounds and update rules

## Transformation

Replace the magnitude-based branching with a single uniform loop that handles all input ranges using one invariant (e.g., decrement a candidate divisor while maintaining residue = M mod candidate)

## Why it helps

Eliminating the input-dependent branch removes a path explosion point and avoids one branch containing a loop with up to 10^7 iterations that KLEE cannot feasibly unroll, letting symbolic execution cover the full input space through a single bounded loop

## Example

Before:

```c
#include <stdio.h>
#include <math.h>
int main()
{	int n,m,t;
	scanf("%d%d",&n,&m);
	if(m-n<10000000)
	{
		while(m%n) n++;
		printf("%d",m/n);
	}
	else
	{
		t=m/n;
		while(m%t) t--;
		printf("%d",t);
	}
	return 0;
}
```

After:

```c
#include <stdio.h>
int main(){
	int N, M;
	scanf("%d %d", &N, &M);
	int estGcd = M / N;
	int residue = M % N;
	while((estGcd) && (residue % estGcd)){
		estGcd--;
		residue += N;
	}
	printf("%d\n", estGcd);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.4444
- Source problems: train_AtCoder_p03241
- Rule id: `d64903aa`
