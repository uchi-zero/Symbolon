---
name: use-arithmetic-range-check
description: Use when code uses direct character comparison against a specific character literal
---

# use-arithmetic-range-check

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses direct character comparison against a specific character literal

## Transformation

Replace character literal comparison with arithmetic range check using subtraction and numeric comparison

## Why it helps

Arithmetic constraints are easier for SMT solvers to reason about than character comparisons, leading to more efficient constraint solving and better path exploration

## Example

Before:

```c
#include<stdio.h>

int main(){
	char x;
	scanf("%c",&x);
	if(x<'a'){
		printf("%c\n",'A');
	}
	else{
		printf("%c\n",'a');
	}
}
```

After:

```c
#include<stdio.h>
int main()
{
  char c;
  c = getchar();
  if(c - 'A' <= 25)
    printf("A");
  else
    printf("a");
}
```

## Evidence

- Average coverage delta: 0.1167
- Source problems: train_AtCoder_p02627
- Rule id: `b4190d70`
