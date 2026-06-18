---
name: data-driven-loop-bounds
description: Use when iterating over a fixed-size buffer with a constant bound and skipping null/sentinel positions inside the loop body
---

# data-driven-loop-bounds

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When iterating over a fixed-size buffer with a constant bound and skipping null/sentinel positions inside the loop body

## Transformation

Replace the constant-bound loop with a pointer/index walk that terminates on the actual data terminator (e.g., null byte), then iterate backward from the true end

## Why it helps

Fixed-bound loops force KLEE to fork on the sentinel check at every iteration regardless of input length, multiplying paths; a data-driven loop creates branches proportional to actual input length, reducing path explosion and making constraints over meaningful indices simpler to solve

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
int main(void){
    char s[22]={'\n'},*p;
    gets(s+1);
    for(p=s;*++p;);
    for(p--;s<=p;p--)putchar(*p);
    return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00006
- Rule id: `22a9dfaa`
