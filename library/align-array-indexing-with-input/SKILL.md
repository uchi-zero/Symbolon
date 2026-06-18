---
name: align-array-indexing-with-input
description: Use when the program accesses a precomputed table using an offset (e.g., arr[n-1]) that introduces an arithmetic operation between the symbolic input and the index
---

# align-array-indexing-with-input

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the program accesses a precomputed table using an offset (e.g., arr[n-1]) that introduces an arithmetic operation between the symbolic input and the index

## Transformation

Reindex the precomputed array so the symbolic input directly maps to the array slot (e.g., store dp[n] instead of x[n-1]), removing the subtraction at the access site

## Why it helps

KLEE models array accesses as constraints over the index expression; eliminating arithmetic on the symbolic index produces simpler, more directly solvable constraints and avoids extra path conditions for boundary cases like n-1 underflow, letting the solver enumerate valid inputs faster and reach more branches.

## Example

Before:

```c
#include<stdio.h>

int main(){

int x[32]={1,2,4};
for(int i=3;i<31;i++)
x[i]=x[i-1]+x[i-2]+x[i-3];

int n;
while(1){
scanf("%d",&n);if(n==0)break;
int d=x[n-1];
int y=d/365/10+1;
printf("%d\n",y);
}
return 0;
}
```

After:

```c
#include<stdio.h>
int main(){
    int i=2,n,dp[31];
    dp[0]=dp[1]=1;
    dp[2]=2;
    while(++i<31)
        dp[i]=dp[i-1]+dp[i-2]+dp[i-3];
    while(scanf("%d",&n),n)
        printf("%d\n",dp[n]/(10*365)+1);
}
```

## Evidence

- Average coverage delta: 0.353
- Source problems: train_AIZU_p00168
- Rule id: `17f85a7d`
