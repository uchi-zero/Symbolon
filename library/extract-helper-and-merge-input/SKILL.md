---
name: extract-helper-and-merge-input
description: Use when code uses inline conditional negation (e.g., `if(X<0)X=-X;`) and multiple sequential scanf calls, creating extra branch points and fragmented input symbolic regions
---

# extract-helper-and-merge-input

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses inline conditional negation (e.g., `if(X<0)X=-X;`) and multiple sequential scanf calls, creating extra branch points and fragmented input symbolic regions

## Transformation

Extract the absolute-value computation into a pure helper function returning a conditional expression, and merge multiple scanf calls into a single combined scanf with one format string

## Why it helps

A pure helper with a ternary expression collapses the abs branch into a single symbolic select rather than a path fork, and a single scanf creates one contiguous symbolic input block, reducing the number of paths KLEE must enumerate and simplifying constraint solving

## Example

Before:

```c
#include <stdio.h>

int main(void)
{
	long long X,K,D,answer,amari;
	scanf("%lld",&X);
	scanf("%lld",&K);
	scanf("%lld",&D);
	if(X<0)X=-1*X;
	if(X/D>K)answer=X-D*K;
	else{
		amari=X%D;
		K-=X/D;
		if(K%2!=0)answer=D-amari;
		else answer=amari;
	}
	printf("%lld",answer);
	return 0;
}
```

After:

```c
#include <stdio.h>
#define LL long long
LL myabs(LL a){
	return a>=0?a:-a;
}
int main(){
	LL x,k,d,ans;
	scanf("%lld%lld%lld",&x,&k,&d);
	x=myabs(x);
	if(x/d>=k)ans=x-k*d;
	else{//x/d<k
		k-=x/d;
		if(k%2==0)ans=x%d;
		else ans=d-x%d;
	}
	printf("%lld\n",ans);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3333
- Source problems: train_AtCoder_p02584
- Rule id: `e02ed223`
