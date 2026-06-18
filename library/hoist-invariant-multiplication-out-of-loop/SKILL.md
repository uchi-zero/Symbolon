---
name: hoist-invariant-multiplication-out-of-loop
description: Use when a loop accumulates a value that is invariant with respect to the loop's symbolic input (e.g., multiplying a constant factor every iteration regardless of the read value)
---

# hoist-invariant-multiplication-out-of-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop accumulates a value that is invariant with respect to the loop's symbolic input (e.g., multiplying a constant factor every iteration regardless of the read value)

## Transformation

Move the input-independent computation out of the loop and compute it once after the loop using a closed-form expression (e.g., pow(3,n)) based on the loop count

## Why it helps

Removing input-independent state updates from the symbolic loop body reduces per-iteration constraint complexity and shrinks the symbolic state, letting KLEE explore branches faster and produce test inputs that generalize across solutions

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
int main(){
int n,a,num=1,m;
	
	
	scanf("%d",&n);
	m=n;
	while(n--)
	{
		scanf("%d",&a);
		if(a%2==1)num*=1;
		else num*=2;
		
	}
	printf("%d",(int)pow(3,m)-num);
	
	
	
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03568
- Rule id: `120787e8`
