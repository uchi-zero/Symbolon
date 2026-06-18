---
name: split-compound-conditions
description: Use when multiple distinct logical cases are merged into a single compound boolean expression using || or && operators
---

# split-compound-conditions

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When multiple distinct logical cases are merged into a single compound boolean expression using || or && operators

## Transformation

Split the compound condition into separate nested if/else branches, with each elementary condition tested individually so each case has its own control-flow path

## Why it helps

KLEE forks execution at each branch; compound conditions short-circuit and collapse multiple semantic cases into fewer paths, hiding sub-conditions from the path explorer. Splitting them exposes each sub-condition as an independent branch, yielding more distinct path constraints and richer test inputs that generalize to other solutions.

## Example

Before:

```c
#include<stdio.h>


int main(){
	int n,a,b;
	scanf("%d%d%d",&n,&a,&b);
	if(a>b||(n==1&&b!=a)){
		printf("0");
	}else{
		printf("%lld\n",(long long )(b-a)*(n-2)+1);
	}
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
- Rule id: `448ee1a8`
