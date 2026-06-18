---
name: avoid-side-effects-in-loop-headers
description: Use when loop counters are initialized outside the loop and side-effecting expressions (like printf with branching arguments) are embedded in the for-statement's increment clause
---

# avoid-side-effects-in-loop-headers

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop counters are initialized outside the loop and side-effecting expressions (like printf with branching arguments) are embedded in the for-statement's increment clause

## Transformation

Move side-effecting computations into the loop body and declare loop variables locally with standard init/condition/increment, separating control flow from output side effects

## Why it helps

KLEE tracks symbolic state more efficiently when branching expressions appear in straightforward statement positions rather than buried in for-loop increment clauses; this exposes branch points clearly to the symbolic executor and reduces path-merging complexity, improving constraint solving and coverage replay

## Example

Before:

```c
#include<stdio.h>
int main(){
  int H,W,h,w,i=1,j;
  scanf("%d%d%d%d",&H,&W,&h,&w);
  if(H%h||W%w){
    for(puts("Yes");i<=H;++i,puts("")) for(j=0;j<W;++j,printf("%d ",(i%h||j%w)?1000:-1000*(h*w-1)-1));
  }else puts("No");
  return 0;
}
```

After:

```c
#include<stdio.h>
int main(){int H,W,h,w;scanf("%d%d%d%d",&H,&W,&h,&w);if(H%h||W%w){puts("Yes");for(int i=1;i<=H;++i,puts("")) for(int j=1;j<=W;++j)printf("%d ",(i%h||j%w)?1000:-1000*(h*w-1)-1);}else puts("No");return 0;}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03689
- Rule id: `68d59b77`
