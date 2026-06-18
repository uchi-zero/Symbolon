---
name: nested-loop-to-single-loop-modulo
description: Use when nested loops check if a linear combination of loop variables equals a target value
---

# nested-loop-to-single-loop-modulo

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops check if a linear combination of loop variables equals a target value

## Transformation

Replace nested loops with a single loop that iterates one variable and uses modulo operation to check if the remaining value is divisible by the other coefficient

## Why it helps

Reduces path explosion from O(n²) nested branches to O(n) branches, and replaces equality constraints with simpler divisibility checks that KLEE can solve more efficiently

## Example

Before:

```c
#include <stdio.h>

int main(){
  int N;
  scanf("%d",&N);
  for(int i=0;i<100;i++){
    for(int j=0;j<100;j++){
      if(4*i+7*j==N) {printf("Yes"); return 0;}
    }
  }
  printf("No");
}
```

After:

```c
#include<stdio.h>

int main()
{
	int n;
	scanf("%d", &n);

	for (int i = 0; i * 4 <= n; i++)
	{
		if (!((n - i * 4) % 7))
		{
			printf("Yes\n");
			return 0;
		}
	}
	printf("No\n");

	return 0;
}
```

## Evidence

- Average coverage delta: 0.1828
- Source problems: train_AtCoder_p03285
- Rule id: `e13b2c2b`
