---
name: avoid-nonlinear-symbolic-constraints
description: Use when the condition involves nonlinear arithmetic over symbolic integers (e.g., multiplications like 4*a*b or squaring (c-a-b)*(c-a-b)) that the SMT solver struggles to reason about
---

# avoid-nonlinear-symbolic-constraints

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the condition involves nonlinear arithmetic over symbolic integers (e.g., multiplications like 4*a*b or squaring (c-a-b)*(c-a-b)) that the SMT solver struggles to reason about

## Transformation

Reformulate the predicate using a concrete library call (e.g., sqrtl) on the symbolic inputs so the comparison becomes effectively linear at the SMT level, replacing multiplied symbolic terms with a single computed value

## Why it helps

KLEE relies on an SMT solver that handles linear integer arithmetic efficiently but times out or imprecisely models nonlinear multiplications of symbolic variables; offloading the nonlinear part to a concrete float computation reduces constraint complexity and lets KLEE explore both branches and generate diverse inputs

## Example

Before:

```c
#include <stdio.h>
int main(){
  long long int a,b,c;
  scanf("%lld%lld%lld", &a, &b, &c);
  if(4*a*b < (c-a-b)*(c-a-b) && c-a-b > 0) {
    puts("Yes");
  }
  else {
    puts("No");
  }
}
```

After:

```c
#include <stdio.h>
#include <math.h>
int main(){
  int a,b,c;
  scanf("%d%d%d",&a,&b,&c);
  puts(a+2*sqrtl((long long)a*b)+b<c ? "Yes" : "No");
  }
```

## Evidence

- Average coverage delta: 0.5444
- Source problems: train_AtCoder_p02743
- Rule id: `b0808d9d`
