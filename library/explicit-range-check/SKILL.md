---
name: explicit-range-check
description: Use when code uses arithmetic operations on character values to check ranges (e.g., c - 97 >= 0)
---

# explicit-range-check

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses arithmetic operations on character values to check ranges (e.g., c - 97 >= 0)

## Transformation

Replace arithmetic-based range checks with explicit character comparisons (e.g., c >= 'A' && c <= 'Z')

## Why it helps

Explicit character comparisons create simpler symbolic constraints that are easier for KLEE to reason about, avoiding complex arithmetic transformations that can hinder constraint solving

## Example

Before:

```c
#include<stdio.h>
 
int main() {
  char a; scanf("%c",&a);
  if (a - 97 >= 0) printf("a");
  else printf("A");
  return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
  int c=getchar();
  if(c>='A'&&c<='Z')return puts("A"),0;
  else return puts("a"),0;
}
```

## Evidence

- Average coverage delta: 0.1667
- Source problems: train_AtCoder_p02627
- Rule id: `eac404fd`
