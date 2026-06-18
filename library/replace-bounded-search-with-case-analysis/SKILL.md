---
name: replace-bounded-search-with-case-analysis
description: Use when code uses nested loops that iterate over a search space derived from input values to find an optimum, causing KLEE to fork on every loop iteration and accumulate path conditions over many symbolic comparisons
---

# replace-bounded-search-with-case-analysis

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When code uses nested loops that iterate over a search space derived from input values to find an optimum, causing KLEE to fork on every loop iteration and accumulate path conditions over many symbolic comparisons

## Transformation

Replace the brute-force enumeration with explicit case analysis using a small number of if/else branches that directly compute the answer based on relationships between input variables

## Why it helps

Loops with input-dependent bounds cause KLEE to explore a combinatorial number of paths and produce complex symbolic constraints; flattening to a few disjoint branches yields a small fixed set of paths with simple linear constraints that the solver handles quickly, improving coverage of distinct behaviors

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int d=1000000000;
	int a,b,c,a1,b1,c1,i,j;
	scanf("%d %d %d",&a,&b,&c);
	a1=c/1000;
	b1=c/500;
	for(i=0;i<a1+2;i++){
		for(j=b1+1;j>=0;j--){
			if(i*1000+j*500>=c){
				c1=i*a+j*b;
				if(c1<=d) d=c1;
			}
		}
	}
	printf("%d\n",d);
	return 0;
}
```

After:

```c
#include<stdio.h>
int main(void)
{
	int a,b,x;
	scanf("%d %d %d",&a,&b,&x);
	while(1){
		if(a<=b){
			if(x%1000==0)	printf("%d\n",a*(x/1000));
			else	printf("%d\n",a*(x/1000+1));
			break;
		}
		if(a>=b*2){
			if(x%500==0)	printf("%d\n",b*(x/500));
			else	printf("%d\n",b*(x/500+1));
			break;
		}
		if(a<b*2){
			if(x%1000==0)	printf("%d\n",a*(x/1000));
			else{
				if(x%1000>500)	printf("%d\n",(a*(x/1000))+a);
				else	printf("%d\n",(a*(x/1000))+b);
			}
			break;
		}
	}
	return 0;
}
```

## Evidence

- Average coverage delta: 0.12
- Source problems: train_AIZU_p00378
- Rule id: `d9c9339f`
