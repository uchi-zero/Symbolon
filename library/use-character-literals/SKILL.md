---
name: use-character-literals
description: Use when comparing character values using numeric ASCII codes (e.g., 97 for 'a')
---

# use-character-literals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When comparing character values using numeric ASCII codes (e.g., 97 for 'a')

## Transformation

Replace numeric ASCII values with character literals (e.g., replace 97 with 'a', 65 with 'A')

## Why it helps

Character literals make the symbolic constraints more explicit and easier for KLEE to reason about, as the solver can better understand the domain of valid character values

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
int main()
{
  char c;
  c = getchar();
  if(c - 'A' <= 25)
    printf("A");
  else
    printf("a");
}
```

## Evidence

- Average coverage delta: 0.1667
- Source problems: train_AtCoder_p02627
- Rule id: `67852f56`
