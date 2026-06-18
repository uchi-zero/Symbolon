---
name: avoid-side-effects-in-for-update
description: Use when a for-loop's update expression uses the comma operator to embed I/O or computation with side effects (e.g., printf with a conditional argument), mixing loop control with branching logic
---

# avoid-side-effects-in-for-update

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a for-loop's update expression uses the comma operator to embed I/O or computation with side effects (e.g., printf with a conditional argument), mixing loop control with branching logic

## Transformation

Move side-effecting calls and conditional expressions out of the for-update clause into the loop body, and use locally-scoped loop variables with standard initialization/increment forms

## Why it helps

KLEE forks on the conditional inside printf; when this conditional is buried in the loop's update expression, path constraints accumulate awkwardly with loop iteration state, and symbolic indices may be harder to concretize. Placing the branch in the loop body yields cleaner per-iteration path conditions and improves constraint solver efficiency and coverage replay

## Example

Before:

```c
#include<stdio.h>
int main(){
  int H,W,h,w,i=1,j;
  scanf("%d%d%d%d",&H,&W,&h,&w);
  if(H%h||W%w){
    puts("Yes");
    for(;i<=H;++i,puts("")) for(j=0;j<W;++j,printf("%d ",(i%h||j%w)?1000:-1000*(h*w-1)-1));
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

- Average coverage delta: 0.6667
- Source problems: train_AtCoder_p03689
- Rule id: `a4341e88`
