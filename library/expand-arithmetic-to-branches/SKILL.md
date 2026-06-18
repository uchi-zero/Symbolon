---
name: expand-arithmetic-to-branches
description: Use when complex arithmetic expressions combine multiple conditions using bitwise/arithmetic operations
---

# expand-arithmetic-to-branches

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When complex arithmetic expressions combine multiple conditions using bitwise/arithmetic operations

## Transformation

Replace arithmetic expressions that encode conditional logic with explicit if-else branches

## Why it helps

KLEE can more easily fork execution paths at explicit branches than solve constraints involving arithmetic encodings of conditions

## Example

Before:

```c
#include <stdio.h>
int main(void){
	int a,b;
	scanf("%d %d",&a,&b);
	printf("%d",a-1 + !(b<a));
}
```

After:

```c
#include<stdio.h>
int main(){
	int m,n;
	while(~scanf("%d%d",&m,&n)){
		if(m>n)
		printf("%d",m-1);
		else
		printf("%d\n",m);
	}
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03359
- Rule id: `2bfbfd16`
