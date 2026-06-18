---
name: remove-input-loop
description: Use when code uses a while loop with scanf to read multiple inputs until EOF
---

# remove-input-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a while loop with scanf to read multiple inputs until EOF

## Transformation

Replace the while loop with a single scanf call that reads one input

## Why it helps

KLEE struggles with loops that depend on symbolic input termination conditions, causing it to explore fewer meaningful paths and get stuck on loop exit conditions rather than the core logic

## Example

Before:

```c
#include<stdio.h>
#include<math.h>
int main()
{
	int n,a;
	float b;
	while(~scanf("%d", &n))
	{
		a=ceil(n/1.08);
		if(floor(a*1.08)==n)
			printf("%d\n", a);
		else 
			printf(":(\n");
	}
	return 0;
}
```

After:

```c
#include <stdio.h>
#include <math.h>

int main(int argc, char const *argv[])
{
  int n,m;

  scanf("%d",&n);
  m=ceil(n/1.08);
  if(n==(int)(m*1.08)) printf("%d",m);
  else printf(":(");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.5
- Source problems: train_AtCoder_p02842
- Rule id: `cc2b550d`
