---
name: nested-loop-to-modular-arithmetic
description: Use when nested loops exhaustively search for integer solutions to linear equations
---

# nested-loop-to-modular-arithmetic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops exhaustively search for integer solutions to linear equations

## Transformation

Replace nested loops with modular arithmetic checks and single loops that reduce the search space by the coefficient of one variable

## Why it helps

Eliminates quadratic path explosion from nested loops and replaces complex loop-dependent constraints with simpler modular arithmetic constraints that KLEE can solve more efficiently

## Example

Before:

```c
#include <stdio.h>

int main(){
  int N;
  scanf("%d",&N);
  for(int i=0;i<100;i++){
    for(int j=0;j<100;j++){
      if(4*i+7*j==N) {printf("Yes"); return 0;}
    }
  }
  printf("No");
}
```

After:

```c
#include <stdio.h>
int main(){
int n;
scanf("%d",&n);
if(n%4==0){
printf("Yes\n");
return 0;
}
while(n>0){
if(n%7==0){
printf("Yes\n");
return 0;
}
n-=4;
}
printf("No\n");
}
```

## Evidence

- Average coverage delta: 0.1828
- Source problems: train_AtCoder_p03285
- Rule id: `49666a29`
