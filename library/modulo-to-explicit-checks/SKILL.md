---
name: modulo-to-explicit-checks
description: Use when code uses modulo operations in conditional expressions
---

# modulo-to-explicit-checks

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses modulo operations in conditional expressions

## Transformation

Replace modulo-based conditions with explicit value checks that capture the same logic

## Why it helps

Modulo operations create complex non-linear constraints that are harder for SMT solvers to reason about, while explicit equality checks create simpler linear constraints that KLEE can solve more efficiently

## Example

Before:

```c
#include<stdio.h>
int main(){
	int a, b;
	scanf("%d%d", &a, &b);
	printf("%s\n", a * b % 2 == 1 ? "Yes" : "No");
}
```

After:

```c
#include<stdio.h>
int main(void){
  int a,b;
  scanf("%d%d",&a,&b);
  if(a==2||b==2)
    printf("No");
  else
    printf("Yes");
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1111
- Source problems: train_AtCoder_p03260
- Rule id: `03f85a52`
