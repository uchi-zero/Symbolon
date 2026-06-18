---
name: simplify-xor-swap
description: Use when code uses XOR swap operations (a^=b^=a^=b) for variable exchange
---

# simplify-xor-swap

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses XOR swap operations (a^=b^=a^=b) for variable exchange

## Transformation

Replace XOR swap with temporary variable assignment (temp=a; a=b; b=temp)

## Why it helps

XOR swaps create complex symbolic expressions that are harder for constraint solvers to reason about, while simple assignments maintain cleaner symbolic state

## Example

Before:

```c
#include<stdio.h>
int main(){char str[5];scanf("%s",str);if(str[2]){str[0]^=str[2]^=str[0]^=str[2]
;}printf("%s",str);return 0;}
```

After:

```c
#include <stdio.h>

int main(){
  char a[3];
  scanf("%s",a);
 
  if(a[2]=='\0'){
    printf("%s",a);
  }else{
    char u = a[0];
    a[0] = a[2];
    a[2] = u;
    printf("%s",a);
  }  
}
```

## Evidence

- Average coverage delta: 0.1389
- Source problems: train_AtCoder_p03227
- Rule id: `c127494d`
