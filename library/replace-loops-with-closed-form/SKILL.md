---
name: replace-loops-with-closed-form
description: Use when the program uses an iterative loop to compute a value that has a known closed-form arithmetic expression
---

# replace-loops-with-closed-form

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program uses an iterative loop to compute a value that has a known closed-form arithmetic expression

## Transformation

Replace the loop (and any auxiliary input reads it depends on) with the equivalent direct arithmetic formula over the input variables

## Why it helps

Loops force KLEE to fork and explore many iterations, multiplying path counts and inflating constraint complexity; a closed-form expression yields a single path with simple linear constraints, dramatically improving coverage and solver speed

## Example

Before:

```c
#include<stdio.h>
int main(){
	int x,N,M,m[300],i;
	int num = 0;
	scanf("%d %d",&N,&M);
	for(i=0;i<M;i++){
		scanf("%d",&m[i]);
	}
	//num????£N/2
	x = N;
	for(;;){
		x = x/2;
		num = num + x;
		if(x == 1) break;
	}
	num = num - M;
	printf("%d\n",num);
	return 0;	
}
```

After:

```c
#include <stdio.h>

int main(void) {
  int a, b;
  scanf("%d%d", &a, &b);
  printf("%d\n", a - b - 1);
  return 0;
}
```

## Evidence

- Average coverage delta: 0.1481
- Source problems: train_AIZU_p01905
- Rule id: `9daedefa`
