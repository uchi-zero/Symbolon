---
name: simplify-boolean-logic
description: Use when multiple equality comparisons are combined with AND/OR operators to check for specific value combinations
---

# simplify-boolean-logic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple equality comparisons are combined with AND/OR operators to check for specific value combinations

## Transformation

Replace compound conditions checking for matching/mismatching values with XOR operations that directly test for difference

## Why it helps

XOR creates simpler constraints with fewer branches, allowing KLEE to more efficiently explore both paths and generate test cases that distinguish between matching and non-matching inputs

## Example

Before:

```c
#include <stdio.h>
int main(void){
  char a,b;
  scanf("%c %c",&a,&b);
  printf("%c\n",((a=='H'&&b=='H')||(a=='D'&&b=='D'))? 'H':'D');
  return 0;
}
```

After:

```c
#include <stdio.h>

int main() {
    char a, b;
    scanf("%c %c", &a, &b);
    printf("%c\n", ((a == 'H') ^ (b == 'H')) ? 'D' : 'H');
}
```

## Evidence

- Average coverage delta: 0.1111
- Source problems: train_AtCoder_p03777
- Rule id: `1ac0d63f`
