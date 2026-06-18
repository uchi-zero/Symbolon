---
name: prefer-parameters-over-globals-in-recursion
description: Use when recursive functions communicate state through mutable global variables (e.g., accumulators, counters) that are modified and restored across recursion levels
---

# prefer-parameters-over-globals-in-recursion

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When recursive functions communicate state through mutable global variables (e.g., accumulators, counters) that are modified and restored across recursion levels

## Transformation

Pass the changing state as function parameters (e.g., remaining target s-i, depth n-1) instead of mutating globals, so each recursive call has self-contained inputs

## Why it helps

KLEE tracks symbolic state more efficiently when values flow through call arguments rather than through aliased global memory; this reduces store/restore side effects, shrinks the symbolic store, and lets the solver prune branches earlier (e.g., terminating when the remaining sum becomes negative) instead of exploring the full combinatorial tree before checking equality

## Example

Before:

```c
#include<stdio.h>
void func(int,int);
int n,m,cnt,sum;
int main(void)
{
	while(1){
		int i,j;
		scanf("%d %d",&n,&m);
		if(n==0&&m==0) break;
		cnt=0; sum=0;
		func(0,0);
		printf("%d\n",cnt);
	}
	return 0;
}
void func(int nn,int x)
{
	int i;
	if(nn==n&&sum==m) cnt++;
	else{
		for(i=x;i<10;i++){
			sum+=i; func(nn+1,i+1);
			sum-=i;
		}
	}	
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

- Average coverage delta: 0.2608
- Source problems: train_AIZU_p00030
- Rule id: `e7dfb4b5`
