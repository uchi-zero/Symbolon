---
name: expand-equality-comparison
description: Use when code directly compares two symbolic variables for equality (a == b)
---

# expand-equality-comparison

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code directly compares two symbolic variables for equality (a == b)

## Transformation

Replace direct equality comparison with explicit comparisons against concrete values using XOR logic: ((a == 'val') ^ (b == 'val'))

## Why it helps

Direct equality between symbolic variables creates complex constraints, while comparing each variable against concrete values creates simpler, independent constraints that KLEE can solve more efficiently and explore more paths

## Example

Before:

```c
#include <stdio.h>

int main() {
  char a, b;
  scanf("%c %c", &a, &b);
  printf("%c\n", a == b ? 'H' : 'D');
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
- Rule id: `db670adc`
