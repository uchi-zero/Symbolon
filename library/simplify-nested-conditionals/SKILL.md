---
name: simplify-nested-conditionals
description: Use when code uses nested if-else with compound AND conditions in the if branch
---

# simplify-nested-conditionals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses nested if-else with compound AND conditions in the if branch

## Transformation

Rewrite using OR conditions with early exit, inverting the logic to test the negative case first

## Why it helps

KLEE can more easily generate test cases for simple disjunctive conditions (OR) than conjunctive ones (AND), as it only needs to satisfy one clause rather than multiple simultaneously

## Example

Before:

```c
#include<stdio.h>
int main()
{
  int a,b;
  scanf("%d %d",&a,&b);
  if(a<=8&&b<=8){
    printf("Yay!\n");
  }else{
    printf(":(\n");
  }
}
```

After:

```c
#include <stdio.h>

int main()
{
  int a, b;
  scanf("%d %d", &a, &b);
  if(a > 8 || b > 8) printf(":(");
  else printf("Yay!");
}
```

## Evidence

- Average coverage delta: 0.1032
- Source problems: train_AtCoder_p03323
- Rule id: `7041bce8`
