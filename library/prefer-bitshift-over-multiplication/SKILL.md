---
name: prefer-bitshift-over-multiplication
description: Use when a loop doubles a symbolic integer using arithmetic multiplication (e.g., X *= 2) as the loop progression operator
---

# prefer-bitshift-over-multiplication

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop doubles a symbolic integer using arithmetic multiplication (e.g., X *= 2) as the loop progression operator

## Transformation

Replace the multiplication-by-2 with a left bit-shift (x <<= 1), and consider promoting loop counters/operands to consistent wide integer types declared at file scope

## Why it helps

KLEE lowers multiplications into more complex symbolic expressions involving the full multiplier, while bit-shifts by a constant become simple, linear constraints that the STP/Z3 solver can resolve much faster, enabling deeper path exploration within time/query budgets

## Example

Before:

```c
#include<stdio.h>

int main(void){
  long long int X,Y;
  scanf("%lld",&X);
  scanf("%lld",&Y);
  int i=0;
  while(X<=Y){
    X *=2;
    i++;
  }
  printf("%d",i);
}
```

After:

```c
#include<stdio.h>
typedef long long ll;
ll s,x,y;
int main(){
    scanf("%lld%lld",&x,&y);
    for(s=0;x<=y;s++){
        x<<=1;
    }
    printf("%lld\n",s);
}
```

## Evidence

- Average coverage delta: 0.4242
- Source problems: train_AtCoder_p03479
- Rule id: `72bf7420`
