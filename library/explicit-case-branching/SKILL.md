---
name: explicit-case-branching
description: Use when code uses a single arithmetic expression with a sign check to handle multiple logical cases (e.g., edge cases like n==1 and ordering conditions like a>b collapsed into one formula)
---

# explicit-case-branching

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses a single arithmetic expression with a sign check to handle multiple logical cases (e.g., edge cases like n==1 and ordering conditions like a>b collapsed into one formula)

## Transformation

Split the computation into explicit if/else branches that separately handle boundary cases and condition-based subcases, assigning the result per branch instead of relying on a unified arithmetic expression

## Why it helps

Explicit branches create distinct control-flow paths that KLEE can fork on, producing separate test inputs for each logical case; a single arithmetic expression yields only one path and forces KLEE to solve complex nonlinear constraints to distinguish cases, reducing coverage diversity

## Example

Before:

```c
#include<stdio.h>
int main(){
	long long a,a1,a2;
	scanf("%lld %lld %lld",&a,&a1,&a2);
	long long t = a1+(a-1)*a2-(a1*(a-1)+a2);
	if(t>=0){
		printf("%lld\n",t+1);
	}
	else printf("0\n");
}
```

After:

```c
#include<stdio.h>
int main(){
long n,a,b,ans;
scanf("%ld%ld%ld",&n,&a,&b);
if(n==1){
if(a==b)
ans=1;
else
ans=0;
}
else{
if(a>b)
ans=0;
else
ans=(n-2)*(b-a)+1;
}
printf("%ld\n",ans);
return 0;
}
```

## Evidence

- Average coverage delta: 0.1111
- Source problems: train_AtCoder_p03705
- Rule id: `591ce5f3`
