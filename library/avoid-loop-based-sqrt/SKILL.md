---
name: avoid-loop-based-sqrt
description: Use when computing square root through iterative loop (while i*i < target)
---

# avoid-loop-based-sqrt

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When computing square root through iterative loop (while i*i < target)

## Transformation

Replace the loop with direct sqrt() function call and integer comparison

## Why it helps

Loops create path explosion in symbolic execution as each iteration generates new constraints, while sqrt() is typically modeled as a single operation

## Example

Before:

```c
#include <stdio.h>
int main(void){
  int a,b,d,t,i=0;
  scanf("%d%d",&a,&b);
  if (b==100) d=1000;
  else if (b<10) d=10;
  else d=100;
  t=a*d+b;
  while (i*i<t) i++;
  printf("%s\n",(i*i==t)? "Yes":"No");
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
- Rule id: `882d2ca0`
