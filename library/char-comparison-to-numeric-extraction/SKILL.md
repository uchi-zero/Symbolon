---
name: char-comparison-to-numeric-extraction
description: Use when multiple character variables are compared for equality using logical OR conditions
---

# char-comparison-to-numeric-extraction

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple character variables are compared for equality using logical OR conditions

## Transformation

Convert character input to a single integer and extract digits using modulo/division operations in a loop, comparing each extracted digit with the previous one

## Why it helps

KLEE handles integer arithmetic and modulo operations more efficiently than character comparisons with multiple symbolic variables, reducing the constraint complexity and enabling better path exploration

## Example

Before:

```c
#include<stdio.h>
int main(){
	char a,b,c,d;
	scanf("%c %c %c %c",&a,&b,&c,&d);
	if (a == b || b == c || c == d){
		printf("Bad\n");
	}
	else{
		printf("Good\n");
	}
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
	int n,a=-1;
	scanf("%d",&n);
	for(int i=1;i<=4;i++){
		if(n%10==a){
			printf("Bad");
			return 0;
		}
		a=n%10;
		n/=10;
	}
	printf("Good");
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3704
- Source problems: train_AtCoder_p02993
- Rule id: `46632311`
