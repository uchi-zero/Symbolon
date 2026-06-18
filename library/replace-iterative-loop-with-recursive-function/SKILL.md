---
name: replace-iterative-loop-with-recursive-function
description: Use when the code uses an iterative loop with mutable state (e.g., Euclidean algorithm via while-loop with multiple variable updates) to compute a mathematical result
---

# replace-iterative-loop-with-recursive-function

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code uses an iterative loop with mutable state (e.g., Euclidean algorithm via while-loop with multiple variable updates) to compute a mathematical result

## Transformation

Refactor the loop into a recursive function with clear base cases and tail-recursive calls, isolating the computation in a separate function with explicit parameters

## Why it helps

Recursive formulations expose path conditions per call frame rather than accumulating loop-iteration constraints, letting KLEE fork on simpler per-call branches and avoid deep loop unrolling that bloats path constraints and degrades constraint-solver performance

## Example

Before:

```c
#include <stdio.h>
int main(void){
  long int n,x,a,b,ans=0,t;
  scanf("%ld%ld",&n,&x);
  a=(x*2<n)? x:n-x;
  b=n-a;
  while (a){
    t=b%a;
    ans+=b-t;
    b=a;
    a=t;
  }
  printf("%ld",ans*3);

  return 0;
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

- Average coverage delta: 0.4037
- Source problems: train_AtCoder_p04048
- Rule id: `6ed60578`
