---
name: split-compound-expressions-and-loop-conditions
description: Use when multiple arithmetic updates are chained via comma operators and assignment expressions on one line, and loop termination is embedded inside a comma-expression scanf condition
---

# split-compound-expressions-and-loop-conditions

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple arithmetic updates are chained via comma operators and assignment expressions on one line, and loop termination is embedded inside a comma-expression scanf condition

## Transformation

Break the compound expression into separate statements (one operation per line), and replace the comma-expression while-condition with an explicit scanf followed by an if-break sentinel check

## Why it helps

Separating statements gives KLEE distinct instruction points and cleaner symbolic state updates per step, while making the input-read and termination check explicit produces a clearer branch on the sentinel value, helping the solver enumerate paths and generate higher-coverage test inputs

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int n,r;
	while(scanf("%d",&n),n)
	{
		n=1000-n;r=n/500;r+=(n%=500)/100;r+=(n%=100)/50;r+=(n%=50)/10;r+=(n%=10)/5;r+=(n%=5);
		printf("%d\n",r);
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

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00444
- Rule id: `d6c03c4a`
