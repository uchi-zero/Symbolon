---
name: explicit-loop-termination-condition
description: Use when a loop's exit condition depends on a complex/aliased expression (e.g., array element compared to a variable in the for-header) that requires symbolic reasoning across many iterations to determine termination
---

# explicit-loop-termination-condition

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When a loop's exit condition depends on a complex/aliased expression (e.g., array element compared to a variable in the for-header) that requires symbolic reasoning across many iterations to determine termination

## Transformation

Replace the implicit termination expression with a simple counter-based loop iteration and add an explicit, semantically clear break condition computed inside the loop body using auxiliary state (e.g., counting zero entries and comparing the current cell to the target)

## Why it helps

KLEE explores paths better when termination conditions are decomposed into simple boolean checks on concrete state; complex header expressions like a[i]-p force the solver to track symbolic array contents across loop back-edges, while explicit in-body breaks produce shorter, more independent path constraints that are easier to solve and replay

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int n,p;
	while(scanf("%d%d",&n,&p),n)
	{
		int i,a[50]={0},m=p;
		for(i=0;a[i]-p;i=(i+1)%n)
		{
			if(m)a[i]++,m--;
			else m=a[i],a[i]=0;
		}
		printf("%d\n",i);
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

- Average coverage delta: 0.2163
- Source problems: train_AIZU_p00740
- Rule id: `c54166ea`
