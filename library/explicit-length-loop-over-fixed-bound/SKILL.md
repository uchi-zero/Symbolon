---
name: explicit-length-loop-over-fixed-bound
description: Use when code iterates over a fixed-size buffer using the buffer's full capacity as loop bounds and conditionally skips null bytes inside the loop
---

# explicit-length-loop-over-fixed-bound

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code iterates over a fixed-size buffer using the buffer's full capacity as loop bounds and conditionally skips null bytes inside the loop

## Transformation

Compute the actual string length first with a dedicated scan loop, then iterate only over the meaningful range without per-iteration null checks

## Why it helps

KLEE forks on every conditional inside the iteration, so a fixed full-buffer loop with an inner null check creates many redundant symbolic branches; computing length first concentrates branching into a single termination condition and reduces the path explosion, yielding tighter constraints and broader coverage when replayed

## Example

Before:

```c
#include<stdio.h>
int main(){
    char str[20]={0},i;
    scanf("%s",&str);
    for(i=19;i>=0;i--){
        if(str[i]!=0)printf("%c",str[i]);
    }
    printf("\n");
    return 0;
}
```

After:

```c
#include <stdio.h>
int main(){
int n=0;
char a[1000];
for(int i=0;i<1000;i++)a[i]='\0';
scanf("%s",a);
while(a[n]!='\0')n++;
for(int i=n-1;i>=0;i--)printf("%c",a[i]);
printf("\n");
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00006
- Rule id: `15bd44ad`
