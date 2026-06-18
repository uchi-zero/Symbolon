---
name: correct-format-specifiers-for-types
description: Use when scanf/printf format specifiers do not match the declared variable types (e.g., using %ld for long long or %d for long long), causing undefined behavior on input parsing
---

# correct-format-specifiers-for-types

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When scanf/printf format specifiers do not match the declared variable types (e.g., using %ld for long long or %d for long long), causing undefined behavior on input parsing

## Transformation

Use type-correct format specifiers (e.g., %lld for long long) so symbolic input is parsed into the intended variable widths and printed consistently

## Why it helps

KLEE models scanf precisely; mismatched specifiers truncate or corrupt symbolic input, leading to spurious paths or aborted exploration. Correct specifiers let KLEE propagate full symbolic values through loop conditions, enabling deeper path coverage and reusable test inputs.

## Example

Before:

```c
#include <stdio.h>
int main(void) {
 long long int i=0,a,X,Y;
scanf("%ld",&X);
scanf("%ld",&Y);
a=X;
while(a<=Y){
 a=a*2;
 i++;
}
printf("%d",i);
 return 0;
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
- Rule id: `040218ce`
