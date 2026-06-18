---
name: input-dependent-loop-bounds
description: Use when loops have fixed bounds that are independent of symbolic input values
---

# input-dependent-loop-bounds

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loops have fixed bounds that are independent of symbolic input values

## Transformation

Replace fixed loop bounds with conditions that depend on the symbolic input, making loops terminate early based on input constraints

## Why it helps

KLEE can prune paths early when loop conditions become false due to input constraints, reducing the number of iterations and path explosion

## Example

Before:

```c
#include <stdio.h>
int f[60005];
int main()
{
	for (int x = 1;x<=100;x++)
		for (int y = 1;y<= 100;y++)
			for (int z = 1;z <= 100;z++)
			{
				int ans = x*x+y*y+z*z+x*y+y*z+x*z;
				f[ans]++;
			}
	int n;
	scanf("%d",&n);
	for (int i = 1;i<= n;i++)
		printf("%d\n",f[i]);
	return 0;
}
```

After:

```c
#include <stdio.h>

int main(void)
{
	int N,x,y,z,answer[10001],i;
	scanf("%d",&N);
	for(i=1;i<=N;i++){
		answer[i]=0;
	}
	for(x=1;x*x<N;x++){
		for(y=1;x*x+y*y<N;y++){
			for(z=1;x*x+y*y+z*z<N;z++){
				if(x*x+y*y+z*z+x*y+y*z+z*x<=N)answer[x*x+y*y+z*z+x*y+y*z+z*x]++;
			}
		}
	}
	for(i=1;i<=N;i++)printf("%d\n",answer[i]);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.25
- Source problems: train_AtCoder_p02608
- Rule id: `46403c5d`
