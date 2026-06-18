---
name: use-strict-inequalities
description: Use when code uses less-than-or-equal (<=) or greater-than-or-equal (>=) operators in conditional expressions
---

# use-strict-inequalities

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses less-than-or-equal (<=) or greater-than-or-equal (>=) operators in conditional expressions

## Transformation

Replace <= with < and >= with > by adjusting the boundary value (e.g., x<=8 becomes x<9, x>=5 becomes x>4)

## Why it helps

Strict inequalities create simpler SMT constraints that are easier for KLEE's constraint solver to reason about, potentially reducing solver time and enabling exploration of more paths within time limits

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
#include<stdio.h>
int main(){int a,b;scanf("%d%d",&a,&b);if(a<9&&b<9)printf("Yay!\n");else printf(":(\n");return 0;}
```

## Evidence

- Average coverage delta: 0.1032
- Source problems: train_AtCoder_p03323
- Rule id: `4fdd0bf7`
