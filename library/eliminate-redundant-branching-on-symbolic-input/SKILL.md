---
name: eliminate-redundant-branching-on-symbolic-input
description: Use when code uses if/else to compute a value that can be expressed as a single branchless arithmetic expression (e.g., computing absolute difference via conditional negation)
---

# eliminate-redundant-branching-on-symbolic-input

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses if/else to compute a value that can be expressed as a single branchless arithmetic expression (e.g., computing absolute difference via conditional negation)

## Transformation

Replace the conditional branches with an equivalent branch-free arithmetic expression that produces the same observable output for the problem's valid input domain

## Why it helps

Each conditional branch on symbolic input forks KLEE's execution state; eliminating unnecessary branches reduces path explosion, lets KLEE generate fewer but more representative test inputs, and avoids spending solver time on constraint paths that don't expose new behavior

## Example

Before:

```c
#include<stdio.h>
int main(){
int i,a,b;
	for(i=0;i<7;i++){
	scanf("%d %d",&a,&b);
	if(b>a){
	b=b-a;
	}
	else{
	b=a-b;
	}
	printf("%d\n",b);
	}
	return 0;
}
```

After:

```c
#include<stdio.h>

int main(void){

	int i=7,a,b;
	while(i!=0){
		scanf("%d %d",&a,&b);
		printf("%d\n",a-b);
		i--;
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AIZU_p00271
- Rule id: `46e4b8a4`
