---
name: split-compound-arithmetic-into-branches
description: Use when the code collapses a conditional mapping (e.g., wrap-around or modulo with offset) into a single arithmetic expression like (a-1)%N+1
---

# split-compound-arithmetic-into-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code collapses a conditional mapping (e.g., wrap-around or modulo with offset) into a single arithmetic expression like (a-1)%N+1

## Transformation

Rewrite the compact arithmetic as an explicit modulo followed by an if-statement that handles the boundary case (e.g., a = a % N; if (a == 0) a = N;)

## Why it helps

Explicit branches create distinct symbolic paths and path constraints that KLEE can enumerate, generating test inputs covering each case; fused arithmetic produces a single path with no branch coverage signal, so replayed inputs miss equivalence classes in other solutions.

## Example

Before:

```c
#include<stdio.h>
int main(){
	int a;
	while(~scanf("%d",&a)){
		printf("3C%02d\n",(a-1)%39+1);
	}
}
```

After:

```c
#include <stdio.h>

int main(void){
		
	int a;
		
	while(1){
		if(scanf("%d", &a) == EOF )
				break;
			
		a = a % 39;
		if(a == 0)	a = 39;
		printf("3C%02d\n", a);
	}
		
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00148
- Rule id: `e6d08f04`
