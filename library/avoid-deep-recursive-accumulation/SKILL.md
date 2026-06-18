---
name: avoid-deep-recursive-accumulation
description: Use when the solution computes a result via deep tail-style recursion that mixes arithmetic accumulation with the base case (e.g., recursing through y/x*x*2 + f(y%x,x) until x==0)
---

# avoid-deep-recursive-accumulation

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the solution computes a result via deep tail-style recursion that mixes arithmetic accumulation with the base case (e.g., recursing through y/x*x*2 + f(y%x,x) until x==0)

## Transformation

Replace the accumulating recursion with a direct closed-form expression using a standard helper (e.g., compute gcd(n,x) and return (n-gcd(n,x))*3) so the main computation is a single arithmetic step

## Why it helps

KLEE forks and accumulates path constraints at every recursive call and branch; deeply recursive arithmetic produces many symbolic paths and complex nonlinear constraints that bog down the solver, while a closed-form expression collapses these into a single path with simple constraints, enabling broader coverage during replay

## Example

Before:

```c
#include<stdio.h>
long long f(long long x,long long y)
{
    if(x==0) return -y;
    else return (y/x)*x*2 + f(y%x,x);
}
int main()
{
    long long n,x;
    scanf("%lld%lld",&n,&x);
    printf("%lld",f(x,n-x)+n);
}
```

After:

```c
#include <stdio.h>
typedef long long LL;
LL n,x;
LL gcd(LL a,LL b){
	if(a<b)a^=b,b^=a,a^=b;
	if(a%b==0) return b;
	return gcd(a%b,b);
}
int main(){
	scanf("%lld%lld",&n,&x);
	printf("%lld",(n-gcd(n,x))*3);
}
```

## Evidence

- Average coverage delta: 0.4857
- Source problems: train_AtCoder_p04048
- Rule id: `92d790a1`
