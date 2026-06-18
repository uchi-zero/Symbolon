---
name: avoid-modulo-operations
description: Use when code uses modulo operator (%) to extract digits or parts of a number
---

# avoid-modulo-operations

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses modulo operator (%) to extract digits or parts of a number

## Transformation

Replace modulo operations with equivalent arithmetic using subtraction and multiplication (e.g., replace 'a%100' with 'a-100*(a/100)')

## Why it helps

Modulo operations create more complex symbolic expressions in KLEE, making constraint solving harder and potentially limiting path exploration, while arithmetic operations with multiplication and subtraction produce simpler constraints

## Example

Before:

```c
#include<stdio.h>

int main(){
int a;scanf("%d",&a);
int L=a/100;
int R=a%100;
if(1<=L&&L<=12){
if(1<=R&&R<=12)printf("AMBIGUOUS\n");
else printf("MMYY\n");
}else{
if(1<=R&&R<=12)printf("YYMM\n");
else printf("NA\n");
}
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

- Average coverage delta: 0.2769
- Source problems: train_AtCoder_p03042
- Rule id: `0e7743cc`
