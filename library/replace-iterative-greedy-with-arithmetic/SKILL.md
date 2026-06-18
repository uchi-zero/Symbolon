---
name: replace-iterative-greedy-with-arithmetic
description: Use when code uses an iterative loop with cascading if/else-if branches to repeatedly subtract fixed denominations, creating many branch decisions and loop iterations per input.
---

# replace-iterative-greedy-with-arithmetic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses an iterative loop with cascading if/else-if branches to repeatedly subtract fixed denominations, creating many branch decisions and loop iterations per input.

## Transformation

Replace the loop and conditional subtraction chain with direct arithmetic using division and modulo operations against each denomination, accumulating the quotient into the counter.

## Why it helps

KLEE forks execution on each branch and loop iteration, so a greedy subtraction loop produces an explosion of paths dependent on the symbolic input value. Straight-line division/modulo code has a single path regardless of input, letting KLEE cover the logic in one execution and generate test inputs that exercise full functionality rather than getting stuck unrolling loops.

## Example

Before:

```c
#include <stdio.h>
int main(){
	int a;
	while(1){
		int n=0;
		scanf("%d",&a);
		if(!a) break;
		a=1000-a;
		while(a>0){
			if(a>=500) a-=500;
			else if(a>=100) a-=100;
			else if(a>=50) a-=50;
			else if(a>=10) a-=10;
			else if(a>=5) a-=5;
			else if(a>=1) a-=1;
			n++;
		}
		printf("%d\n",n);
	}
	return 0;
}
```

After:

```c
#include<stdio.h>

int main(void){
	int n,k;

	while(0==0){
		scanf("%d",&n);
		if(n==0)break;
		n=1000-n;
		k=0;
		k += n /500;  
		n =n%500;
		k += n/100;   
		n =n%100;
		k += n/50;    
		n =n%50;
		k += n/10;    
		n =n%10;
		k += n/5;     
		k += n%5;
		printf("%d\n",k);
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.5704
- Source problems: train_AIZU_p00444
- Rule id: `3e2fbf48`
