---
name: fixed-to-dynamic-loops
description: Use when a loop has a fixed iteration count that processes variable-length data (e.g., digits of a number)
---

# fixed-to-dynamic-loops

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop has a fixed iteration count that processes variable-length data (e.g., digits of a number)

## Transformation

Replace the fixed-iteration loop with a while loop that continues until the data is fully processed (e.g., while n > 0)

## Why it helps

Fixed loops force symbolic execution to explore all iterations even when unnecessary, while dynamic loops allow early termination based on actual data, reducing path explosion and constraint complexity

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

int main(void){
  int n, c;

  scanf("%d", &n);

  c = 0;
  while (n > 0) {
    if (n % 10 == 2) {
      c++;
    }
    n /= 10;
  }

  printf("%d\n", c);
}
```

## Evidence

- Average coverage delta: 0.3687
- Source problems: train_AtCoder_p03192
- Rule id: `282e1ec5`
