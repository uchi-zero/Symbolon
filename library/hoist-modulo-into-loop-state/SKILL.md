---
name: hoist-modulo-into-loop-state
description: Use when a loop iterates a candidate divisor and tests divisibility via a modulo on the original input each iteration (e.g., m % i == 0 inside a decrementing loop)
---

# hoist-modulo-into-loop-state

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop iterates a candidate divisor and tests divisibility via a modulo on the original input each iteration (e.g., m % i == 0 inside a decrementing loop)

## Transformation

Replace the per-iteration modulo on the original variable with an incrementally maintained residue: precompute residue = M % N once, then in each loop step update residue += N and decrement the candidate, testing residue % estGcd

## Why it helps

KLEE's symbolic constraint solver handles modulo operations on accumulating linear expressions more efficiently than repeated modulo against a single large symbolic input; the incremental residue keeps operand magnitudes small and constraints simpler, enabling deeper path exploration within solver timeouts

## Example

Before:

```c
#include <stdio.h>
int main()
{
	int n,m;
	while(scanf("%d%d", &n, &m)!=EOF){
		for(int i = m/n; i >= 1; i--){
			if(m % i == 0) {
				printf("%d\n",i); 
				break;
			}
		}
	}
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

- Average coverage delta: 0.6296
- Source problems: train_AtCoder_p03241
- Rule id: `68cf6318`
