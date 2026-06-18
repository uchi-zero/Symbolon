---
name: avoid-iterative-gcd-with-mutated-params
description: Use when GCD/LCM is computed via an iterative loop that mutates the input parameters and reuses them as both state and divisor, creating loop-carried symbolic dependencies
---

# avoid-iterative-gcd-with-mutated-params

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When GCD/LCM is computed via an iterative loop that mutates the input parameters and reuses them as both state and divisor, creating loop-carried symbolic dependencies

## Transformation

Replace the iterative parameter-mutating gcd loop with a clean recursive gcd(a,b) returning gcd(b, a%b), and express lcm as (a/gcd(a,b))*b using fresh local computation

## Why it helps

Recursive gcd produces simpler, smaller per-call symbolic constraints (each call introduces one modulo expression) instead of a loop whose path condition accumulates compounded modulo expressions over mutated variables, letting KLEE's solver discharge queries faster and reach more downstream paths

## Example

Before:

```c
#include<stdio.h>

int lcm(int a,int b){
    int m;
    int p=a*b;
    m = a%b;
    while(m>0){
        a = b;
        b = m;
        m = a%b;
    }
    return p/b;
}

int main(){
    int n,a,b;
    scanf("%d",&n);
    scanf("%d %d",&a,&b);
    a = lcm(a,b);
    while(n>2){
        scanf("%d",&b);
        a = lcm(a,b);
        n--;
    }
    printf("%d\n",a);
}
```

After:

```c
#include <stdio.h>

int gcd(int a, int b)
{
    if(b==0) return a;
    return gcd(b, a%b);
}

int lcm(int a, int b)
{
    return (a/gcd(a,b))*b;
}

int a[20];

int main()
{
    int n, l, i;
    while(scanf("%d", &n)==1){
        l = 0;
        scanf("%d", &a[0]);
        scanf("%d", &a[1]);
        l = lcm(a[0], a[1]);
        for(i=2; i<n; i++){
            scanf("%d", &a[i]);
            l = lcm(l, a[i]);
        }
        printf("%d\n", l);
    }
    return 0;
}
```

## Evidence

- Average coverage delta: 0.4234
- Source problems: train_AIZU_p02469
- Rule id: `e689ad65`
