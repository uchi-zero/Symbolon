---
name: nested-loop-to-single-loop-division
description: Use when nested loops are used to find factor pairs by testing all combinations
---

# nested-loop-to-single-loop-division

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When nested loops are used to find factor pairs by testing all combinations

## Transformation

Replace nested loops with a single loop that uses division and modulo operations to check if a number is a valid factor

## Why it helps

Single loop with arithmetic operations creates simpler path conditions and constraints for KLEE compared to nested loops, reducing the symbolic execution state space and making constraint solving more efficient

## Example

Before:

```c
#include<stdio.h>
int main(void) {  int qq, n, i, u;scanf("%d", &n);  for(i = 1;i <= 9;i++) {for(u = 1;u <= 9;u++) { qq = i * u; if(qq == n){puts("Yes"); return 0;}}}puts("No");return 0;}
```

After:

```c
#include <stdio.h>
int main() {
	int N, i;
	scanf("%d", &N);
	for (i = 1; i < 10; i++) {
		if ((N / i < 10) && (N%i == 0)) {
			printf("Yes\n");
			break;
		}
	}
	if (i == 10)
		printf("No\n");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1455
- Source problems: train_AtCoder_p02880
- Rule id: `e01d999c`
