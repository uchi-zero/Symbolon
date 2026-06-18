---
name: fixed-to-dynamic-loop
description: Use when a loop has a fixed iteration count that processes variable-length data (e.g., digits of a number)
---

# fixed-to-dynamic-loop

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop has a fixed iteration count that processes variable-length data (e.g., digits of a number)

## Transformation

Replace the fixed-iteration loop with a while loop that terminates based on the actual data condition (e.g., while(n) instead of for(i=0;i<4;i++))

## Why it helps

KLEE can terminate path exploration early when the data condition is met (e.g., n becomes 0), avoiding unnecessary symbolic execution of remaining iterations that would process zeros, reducing constraint complexity and path explosion

## Example

Before:

```c
#include<stdio.h>

int main()
{
  int n;
  scanf("%d",&n);
  int s=0;
  for (int i=0;i<4;i++)
  {
    if (n%10==2) s++;
    n=n/10;
  }
  printf("%d\n",s);
  return 0;
}
```

After:

```c
#include <stdio.h>

int main()
{
  int n;
  scanf("%d", &n);
  int ans =0;
  while(n)
  {
    int tmp = n%10;
    if (tmp ==2) ans++;
    n/=10;
  }
  printf("%d\n", ans);


}
```

## Evidence

- Average coverage delta: 0.3434
- Source problems: train_AtCoder_p03192
- Rule id: `a97ea22d`
