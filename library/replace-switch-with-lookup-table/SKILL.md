---
name: replace-switch-with-lookup-table
description: Use when code uses a switch/if-else chain to map a small integer key to a constant value, creating multiple distinct control-flow branches
---

# replace-switch-with-lookup-table

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a switch/if-else chain to map a small integer key to a constant value, creating multiple distinct control-flow branches

## Transformation

Replace the switch statement with an array indexed by the key, so the mapping becomes a single data-dependent expression rather than branching control flow

## Why it helps

KLEE forks a separate execution state for each switch case, multiplying path counts and constraint solver calls; an array lookup keeps execution on a single path and encodes the mapping as a symbolic memory read, dramatically reducing path explosion and improving coverage per unit time

## Example

Before:

```c
#include<stdio.h>

int main(){

int x,y;

//while(scanf("%d %d",&x,&y)!=EOF){

for(int i=0;i<4;i++){
scanf("%d %d",&x,&y);

int t=0;
switch(x){
case 1:
t=6000;break;

case 2:
t=4000;break;

case 3:
t=3000;break;

case 4:
t=2000;break;
}

printf("%d\n",t*y);
}
return 0;
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int i, j, k, a, b, p[5] = {0, 6000, 4000, 3000, 2000};
  for(i = 0; i < 4; ++i) {
    scanf("%d%d", &a, &b);
    printf("%d\n", p[a] * b);
  }
  return 0;
}
```

## Evidence

- Average coverage delta: 0.2281
- Source problems: train_AIZU_p00272
- Rule id: `25ae2ce7`
