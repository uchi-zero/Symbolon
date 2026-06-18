---
name: avoid-array-indexing-arithmetic
description: Use when input is read into an array and then accessed with arithmetic operations to form values
---

# avoid-array-indexing-arithmetic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When input is read into an array and then accessed with arithmetic operations to form values

## Transformation

Read input as a single integer and extract components using division and modulo operations

## Why it helps

Array indexing with arithmetic creates complex symbolic expressions that are harder for KLEE to reason about, while integer arithmetic operations produce simpler constraints

## Example

Before:

```c
#include <stdio.h>
int arr[4];
int main(){
	for(int i=0;i<4;i++) scanf("%1d",arr+i);
	int a=arr[0]*10+arr[1], b=arr[2]*10+arr[3];
	int s=0;
	if(a<=12&&a>0) s++;
	if(b<=12&&b>0) s+=2;
	if(s==0) printf("NA");
	else if(s==3) printf("AMBIGUOUS");
	else if(s==1) printf("MMYY");
	else printf("YYMM");
}
```

After:

```c
#include<stdio.h>
int main()
{
  int a, b, c, d, e;
  scanf("%d", &e);
  a=e/100;
  b=e-100*a;
  if(a>=1&&a<=12)
  {
  	if(b>=1&&b<=12)
  	printf("AMBIGUOUS");
  	else
  	printf("MMYY");
  }
  else
  {
  	if(b>=1&&b<=12)
  	printf("YYMM");
  	else
  	printf("NA");
  }
}
```

## Evidence

- Average coverage delta: 0.1735
- Source problems: train_AtCoder_p03042
- Rule id: `8d2c1bd2`
