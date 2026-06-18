---
name: replace-floating-point-with-integer-logic
description: Use when using floating-point operations (pow, log10) for integer computations
---

# replace-floating-point-with-integer-logic

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When using floating-point operations (pow, log10) for integer computations

## Transformation

Replace floating-point calculations with equivalent integer operations using conditional logic based on value ranges

## Why it helps

Integer arithmetic with explicit conditionals creates simpler path constraints that KLEE can solve more efficiently than complex floating-point operations

## Example

Before:

```c
// Your code here!
#include <stdio.h>
#include <math.h>
double a,b,c;
int main(){
scanf("%lf %lf",&a,&b);
c=floor(log10(b)+1);
c=a*pow(10,c)+b;
if(pow(floor(sqrt(c)),2)==c){
printf("Yes");
}else{
printf("No");
}
return 0;
}
```

After:

```c
#include<stdio.h>
#include<math.h>
int main(){
int a,b;
scanf("%d%d",&a,&b);
if(b>=100)a=a*1000+b;
else if(b>=10)a=a*100+b;
else a=a*10+b;
b=sqrt(a+0.1);
if(b*b==a)puts("Yes");
else puts("No");
return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p03456
- Rule id: `0557a5b9`
