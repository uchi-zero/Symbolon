---
name: simplify-modulo-expressions
description: Use when extracting middle digits using nested arithmetic operations like (a%100)/10
---

# simplify-modulo-expressions

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When extracting middle digits using nested arithmetic operations like (a%100)/10

## Transformation

Replace with simpler sequential division and modulo: (a/10)%10

## Why it helps

Simpler arithmetic expressions reduce constraint complexity, making it easier for KLEE's constraint solver to reason about path conditions and generate test inputs

## Example

Before:

```c
#include <stdio.h>

int main () {
	int a;
	scanf("%d",&a);
	if ((a%100)/10==7 || a/100==7 || (a%10)==7) {
		printf("Yes");
	}
	else printf("No");
	
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
	int n;
	scanf("%d" , &n);
	if (n%10 == 7 || (n/10) %10 == 7 || n/100 == 7) {
		puts("Yes\n");
	} 
	else{
		puts("No\n");
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.1279
- Source problems: train_AtCoder_p02711
- Rule id: `e3c60f62`
