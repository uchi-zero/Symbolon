---
name: avoid-arithmetic-comparisons
description: Use when code uses arithmetic operations (like subtraction) to check character ranges or ASCII values
---

# avoid-arithmetic-comparisons

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses arithmetic operations (like subtraction) to check character ranges or ASCII values

## Transformation

Replace arithmetic comparisons with direct character literal comparisons (e.g., replace 'a - 97 >= 0' with 'a >= 'a'')

## Why it helps

Direct character comparisons create simpler symbolic constraints that are easier for KLEE to reason about, while arithmetic operations introduce additional symbolic expressions that complicate constraint solving

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
#include <stdio.h>

int main()
{
  char S[10]; scanf ("%s",S);
  putchar(S[0]>='a'?'a':'A');
return 0;
}
```

## Evidence

- Average coverage delta: 0.1667
- Source problems: train_AtCoder_p02627
- Rule id: `4e04bb42`
