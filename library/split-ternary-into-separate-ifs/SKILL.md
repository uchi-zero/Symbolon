---
name: split-ternary-into-separate-ifs
description: Use when a single output statement uses a ternary/conditional expression on a symbolic value to choose between distinct computations
---

# split-ternary-into-separate-ifs

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a single output statement uses a ternary/conditional expression on a symbolic value to choose between distinct computations

## Transformation

Replace the inline conditional expression with separate top-level if statements, each guarding its own branch and side-effect (e.g., its own printf)

## Why it helps

Separate if statements create distinct basic blocks and explicit branch instructions at the LLVM level, so KLEE forks cleanly on each condition and generates a dedicated test input per branch; ternary expressions can be lowered to select instructions or merged paths, hiding branch points and reducing per-branch coverage

## Example

Before:

```c
#include <stdio.h>

int main(){
	int a, b;
	char op;
	
	scanf("%d %c %d", &a, &op, &b);
	printf("%d\n", op=='+'?(a+b):(a-b));
	return 0;
}
```

After:

```c
#include<stdio.h>
int main()
{
	int  A,B;
	char op;
	scanf("%d %c %d",&A,&op,&B);
	if(op==43)
	printf("%d",A+B); 
	if(op==45)
	printf("%d",A-B);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1072
- Source problems: train_AtCoder_p03844
- Rule id: `695e11fb`
