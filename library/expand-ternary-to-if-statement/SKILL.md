---
name: expand-ternary-to-if-statement
description: Use when conditional expressions use ternary operators with complex conditions
---

# expand-ternary-to-if-statement

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When conditional expressions use ternary operators with complex conditions

## Transformation

Replace ternary operator (?:) with explicit if-else statements, computing the default value first then conditionally overwriting it

## Why it helps

KLEE can more easily fork execution paths at explicit branch points (if statements) than when analyzing ternary operators, leading to better path exploration and simpler constraint generation

## Example

Before:

```c
#include<stdio.h>

int main() { int a, b; scanf("%d %d", &a, &b); printf("%d\n", (a>9|b>9)?-1:a*b);}
```

After:

```c
#include<stdio.h>
int main(){
  int A,B,C;
  scanf("%d%d",&A,&B);
  C=A*B;
  if(A>=10||B>=10){
    C=-1;
  }
  printf("%d\n",C);
}
```

## Evidence

- Average coverage delta: 0.625
- Source problems: train_AtCoder_p02879
- Rule id: `bfda47d0`
