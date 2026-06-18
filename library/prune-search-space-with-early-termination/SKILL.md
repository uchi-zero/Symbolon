---
name: prune-search-space-with-early-termination
description: Use when recursion enumerates a fixed-size combinatorial space (e.g., always iterating to depth 10) and only checks the target condition at the leaves, causing exponential path explosion regardless of input
---

# prune-search-space-with-early-termination

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When recursion enumerates a fixed-size combinatorial space (e.g., always iterating to depth 10) and only checks the target condition at the leaves, causing exponential path explosion regardless of input

## Transformation

Restructure recursion to use the input parameters as the recursion bound (e.g., decrement n until zero) and prune branches via a loop with monotonically advancing indices, so the search tree shrinks based on input constraints rather than a fixed depth

## Why it helps

KLEE forks on every branch in the recursion tree; a fixed-depth full enumeration creates 2^10 paths per query independent of inputs, while an input-bounded recursion lets the symbolic executor prune infeasible subtrees early and reach distinct output states with far fewer path explorations

## Example

Before:

```c
///Sum of integers
#include<stdio.h>

int N,S;

int f(int n,int m,int s){
    if(m==10){
        if(S==s&&N==n)return 1;
        return 0;
    }

    return f(n,m+1,s)+f(n+1,m+1,s+m);
}
int main(void){
    while(1){
        scanf("%d%d",&N,&S);
        if(N==0&&S==0)break;

        printf("%d\n",f(0,0,0));
    }
    return 0;
}
```

After:

```c
#include <stdio.h>
int ans;
void f(int n,int l,int s){
	if(n==0){
		if(s==0)ans++;
		return;
	}
	for(int i=l;i<=9;i++)f(n-1,i+1,s-i);
	return;
}
int main(){
	int n,s;
	while(1){
		scanf("%d%d",&n,&s);
		if(n==0&&s==0)return 0;
		ans=0;
		f(n,0,s);
		printf("%d\n",ans);
	}
}
```

## Evidence

- Average coverage delta: 0.283
- Source problems: train_AIZU_p00030
- Rule id: `c827a993`
