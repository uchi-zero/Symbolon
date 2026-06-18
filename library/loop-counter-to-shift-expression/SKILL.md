---
name: loop-counter-to-shift-expression
description: Use when a loop uses multiplication/division by powers of 2 with a counter that starts at 1
---

# loop-counter-to-shift-expression

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop uses multiplication/division by powers of 2 with a counter that starts at 1

## Transformation

Replace multiplication-based loop counter with bit shift operations and adjust loop to use shift-based conditions

## Why it helps

Bit shifts create simpler symbolic expressions than multiplication/division, making constraint solving more efficient and enabling KLEE to explore paths with less computational overhead

## Example

Before:

```c
#include <stdio.h>
int main()
{
    int a;
    scanf("%d",&a);
    int i;
    for(i=1;i<=a;i=i*2)
    {}
    printf("%d",i/2);
}
```

After:

```c
#include <stdio.h>

int main()
{
	int n, i;
	
	scanf("%d", &n);
	
	for(i=0;1<<(i+1)<=n;i++);
	
	printf("%d", 1<<i);
	
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03644
- Rule id: `a2299275`
