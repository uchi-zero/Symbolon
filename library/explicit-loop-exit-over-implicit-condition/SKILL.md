---
name: explicit-loop-exit-over-implicit-condition
description: Use when a while loop uses an implicit truthiness check on a variable that is also modified inside the loop body (e.g., while(a) with a /= 5)
---

# explicit-loop-exit-over-implicit-condition

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a while loop uses an implicit truthiness check on a variable that is also modified inside the loop body (e.g., while(a) with a /= 5)

## Transformation

Rewrite as an infinite while(1) loop with an explicit if(var == 0) break placed after the variable update, making the termination condition a distinct branch

## Why it helps

KLEE forks on explicit branch instructions; an explicit zero-check creates a clearly separated path constraint per iteration, helping the solver enumerate loop-exit paths and improving branch coverage when replayed, rather than folding the exit test into the loop header's implicit condition

## Example

Before:

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  long long a, ans;
  while( 1 ) {
    ans = 0;
    scanf("%lld", &a);
    if(!a) break;
    while(a) {
      ans += a / 5;
      a /= 5;
    }
    printf("%lld\n", ans);
  }
  return 0;
}
```

After:

```c
#include <stdio.h>

int main(void){
	int n;
	int ans;

	while(1){
		ans = 0;
		scanf("%d", &n);
		if(n == 0) break;

		while(1){
			ans = ans + n / 5;
			n = n / 5;
			if(n == 0) break;
		}

		printf("%d\n", ans);
	}

	return 0;
}
```

## Evidence

- Average coverage delta: 0.2083
- Source problems: train_AIZU_p00052
- Rule id: `b5b06b24`
