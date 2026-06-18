---
name: early-exit-special-cases
description: Use when code uses array-based cycle detection or complex termination conditions that depend on symbolic memory state
---

# early-exit-special-cases

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses array-based cycle detection or complex termination conditions that depend on symbolic memory state

## Transformation

Replace array-based cycle detection with direct checks for known cycle values (like 1, 2, 4 in Collatz sequence) and use early returns instead of loop termination conditions

## Why it helps

KLEE struggles with symbolic array indexing and complex loop termination conditions - direct value checks create simpler path conditions and avoid symbolic memory operations, enabling deeper exploration

## Example

Before:

```c
#include <stdio.h>
int a[10000];
int x, n;
int main() {
	scanf("%d", &x);
	while (a[x] < 2) {
		a[x]++;
		if (x % 2)x = 3 * x + 1;
		else x /= 2;
		n++;
	}
	printf("%d\n", n - 2);
}
```

After:

```c
#include <stdio.h>

int main()
{
    int s;
    scanf("%d",&s);
    for(int i=1;;i++)
    {
        if(s==1||s==2||s==4)
        {
            printf("%d\n",i+3);
            return 0;
        }
        if(s%2)s=3*s+1;
        else s=s/2;
    }
}
```

## Evidence

- Average coverage delta: 0.8846
- Source problems: train_AtCoder_p03146
- Rule id: `c9dd6f95`
