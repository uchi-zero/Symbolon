---
name: char-to-int-comparison
description: Use when character variables are used for comparison operations
---

# char-to-int-comparison

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When character variables are used for comparison operations

## Transformation

Replace char type variables with int type and use appropriate format specifiers (e.g., %X for hex input instead of %c)

## Why it helps

Integer comparisons generate simpler constraints for KLEE's constraint solver than character comparisons, and avoiding character-specific handling reduces symbolic execution complexity

## Example

Before:

```c
#include<stdio.h>
int main(){
char X,Y;
scanf("%c %c",&X,&Y);
if(X<Y){
	printf("<\n");
}else if(X>Y){
	printf(">\n");
}else{
	printf("=\n");
}


}
```

After:

```c
#include <stdio.h>
int main()
{
  int a,b;
  scanf("%X %X",&a,&b);
  if(a>b)printf(">");
  else if(a<b)printf("<");
  else printf("=");
}
```

## Evidence

- Average coverage delta: 0.1076
- Source problems: train_AtCoder_p03547
- Rule id: `25d9e10f`
