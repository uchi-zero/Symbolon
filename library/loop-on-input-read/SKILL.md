---
name: loop-on-input-read
description: Use when main reads input only once via scanf and exits, limiting symbolic exploration to a single input vector per execution
---

# loop-on-input-read

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When main reads input only once via scanf and exits, limiting symbolic exploration to a single input vector per execution

## Transformation

Wrap the scanf and processing logic in a while(~scanf(...)) loop so the program repeatedly consumes inputs until EOF, and avoid intermediate temporary variables by inlining expressions

## Why it helps

KLEE can fork on the scanf return value, exploring both the EOF-termination path and the continued-reading path, which exercises more program states and yields test inputs covering multiple iterations and boundary conditions

## Example

Before:

```c
#include<stdio.h>
int main(){
	int x1, y1, x2, y2,Xj,Yj;
	scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
	Xj = x2 - x1;
	Yj = y2 - y1;
	printf("%d %d %d %d\n",x2-Yj,y2+Xj,x1-Yj,y1+Xj);
	return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
	int x1, x2, x3, x4, y1, y2, y3, y4;
	while(~scanf("%d %d %d %d", &x1, &y1, &x2, &y2))
		printf("%d %d %d %d\n", y1-y2+x2, x2-x1+y2, y1-y2+x1, x2-x1+y1);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03265
- Rule id: `53906f20`
