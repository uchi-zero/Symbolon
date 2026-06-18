---
name: replace-data-dependent-loop-exit-with-explicit-state-check
description: Use when loop termination depends on a single data-dependent condition (e.g., a counter equaling a target) reached via accumulated mutations, KLEE must symbolically simulate many iterations before any exit branch becomes feasible.
---

# replace-data-dependent-loop-exit-with-explicit-state-check

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When loop termination depends on a single data-dependent condition (e.g., a counter equaling a target) reached via accumulated mutations, KLEE must symbolically simulate many iterations before any exit branch becomes feasible.

## Transformation

Rewrite the termination test as an explicit scan over the full state (e.g., counting empty slots and checking the active slot equals the target), so the exit condition is expressed as a structural invariant over all array elements rather than a single local equality.

## Why it helps

Expressing termination as an aggregate predicate over all state variables produces richer branch conditions per iteration, giving KLEE more distinct path constraints to fork on and enabling its solver to prune or reach exit states faster, improving coverage.

## Example

Before:

```c
#include <stdio.h>
int main()
{
  int t[50],n,s,storn,i,j;
  while(1){
    scanf("%d %d",&n,&storn);
    //printf("%d %d",n,storn);///
    if(n==0&&storn==0)break;
    for(i=0;i<n;i++)t[i]=0;
    j=0;
    s=storn;
    while(1){
      //printf("%d %d %d %d\n",j,s,t[j],storn);///
      if(s!=0){
	t[j]++;
	s--;
      }else if(s==0){
	s=t[j];
	t[j]=0;
      }
      if(t[j]==storn)break;
      j++;
      if(j==n)j=0;
    }
    printf("%d\n",j);
  }
  return 0;
}
```

After:

```c
#include<stdio.h>

int main(){
	int n,t=0,a[64],k,s,i,j;
	while(1){
		scanf("%d %d",&n,&s);
		if(n==0&&s==0)break;
		k=s;
		for(i=0;i<64;i++)a[i]=0;
		for(i=0;i<n;i=(i+1)%n){
			//for(j=0;j<n;j++)printf("%d ",a[j]);
			//printf("\n");
			if(k){
				a[i]++;
				k--;
			}else if(a[i]){
				k=a[i];
				a[i]=0;
			}
			for(t=0,j=0;j<n;j++)if(!a[j])t++;
			if(t==n-1&&a[i]==s)break;
		}
		printf("%d\n",i);
	}	
	return 0;
}
```

## Evidence

- Average coverage delta: 0.2769
- Source problems: train_AIZU_p00740
- Rule id: `1843c973`
