---
name: xor-swap-to-explicit-logic
description: Use when code uses XOR swap operations (a^=b^=a^=b) for conditional transformations
---

# xor-swap-to-explicit-logic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses XOR swap operations (a^=b^=a^=b) for conditional transformations

## Transformation

Replace XOR swap with explicit conditional logic that directly implements the intended behavior (e.g., reversing string with a loop)

## Why it helps

XOR swap creates complex symbolic expressions that are harder for KLEE to reason about, while explicit loops and direct character access generate simpler constraints that lead to better path exploration

## Example

Before:

```c
#include<stdio.h>
int main(){char str[5];scanf("%s",str);if(str[2]){str[0]^=str[2]^=str[0]^=str[2]
;}printf("%s",str);return 0;}
```

After:

```c
#include<stdio.h>
int main(){
  char s[4];
  scanf("%s",s);
  if(s[2]=='\0'){
    printf("%s",s);
  }else{
    int i;
    for(i=2;i>=0;i--){
      printf("%c",s[i]);
    }
  }
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1282
- Source problems: train_AtCoder_p03227
- Rule id: `33b27188`
