---
name: explicit-block-bodies-for-conditionals
description: Use when if-statements use single-statement bodies without braces and combine increment with the conditional check on one line
---

# explicit-block-bodies-for-conditionals

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When if-statements use single-statement bodies without braces and combine increment with the conditional check on one line

## Transformation

Wrap conditional bodies in explicit braces with the state update as a separate statement (e.g., 'if(cond){ z = z + 1; }' instead of 'if(cond)count++;')

## Why it helps

Explicit block structure produces clearer basic-block boundaries in the compiled LLVM IR, helping KLEE distinguish path branches and generate test inputs that exercise both the true and false sides of each condition, improving branch coverage when replayed.

## Example

Before:

```c
#include<stdio.h>
int main(){
int a,b,c,i,j,k,x,count=0;
scanf("%d%d%d%d",&a,&b,&c,&x);
for(i=0;i<=a;i++){
for(j=0;j<=b;j++){
for(k=0;k<=c;k++){
if(i*500+j*100+k*50==x)count++;
}}}
printf("%d",count);
return 0;
}
```

After:

```c
#include <stdio.h>
int main(void)
{
	int a,b,c,x,i,j,k,z=0;
	scanf("%d%d%d%d",&a,&b,&c,&x);
	for(i=0; i<=a; i++){
		for(j=0; j<=b; j++){
			for(k=0; k<=c; k++){
				if(i*500+j*100+k*50==x){
					z=z+1;
				}
			}
		}
	}
	printf("%d\n",z);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.2896
- Source problems: train_AtCoder_p03448
- Rule id: `ab138d5a`
