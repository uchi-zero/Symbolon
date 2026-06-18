---
name: char-to-int-conversion
description: Use when multiple character inputs are read and compared individually
---

# char-to-int-conversion

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple character inputs are read and compared individually

## Transformation

Convert character sequence input to a single integer and process digits using modulo/division operations

## Why it helps

Integer arithmetic with modulo/division creates simpler constraints for KLEE than multiple character comparisons, reducing the constraint solver complexity and enabling more efficient path exploration

## Example

Before:

```c
#include<stdio.h>
int main(){
	char a, b, c, d;
	scanf("%c%c%c%c", &a, &b, &c, &d);
	if (a == b || b == c || c == d)printf("Bad\n");
	else printf("Good\n");

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

- Average coverage delta: 0.375
- Source problems: train_AtCoder_p02993
- Rule id: `aacaa6e4`
