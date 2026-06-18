---
name: flatten-struct-to-array
description: Use when state is stored in arrays of multi-field structs with assignments and modular arithmetic intermixed across cells
---

# flatten-struct-to-array

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When state is stored in arrays of multi-field structs with assignments and modular arithmetic intermixed across cells

## Transformation

Replace the array-of-struct representation with a multi-dimensional integer array indexed by an extra dimension for the former struct fields, and apply transformations (e.g., modulo) in a uniform loop over that dimension

## Why it helps

KLEE handles flat integer array accesses with concrete indices more efficiently than struct copies and field-wise updates; uniform loop-based updates produce simpler, more symmetric constraints and fewer distinct memory objects, enabling deeper path exploration within the solver's time budget

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int w,h,i,j,N=100000;
	while(scanf("%d%d",&w,&h),w)
	{
		struct S{int e1,e2,n1,n2;}a[102][102],s;
		s.e1=s.e2=s.n1=s.n2=0;
		for(i=0;i<h;++i)for(j=0;j<w;++j)a[i][j]=s;
		a[0][0].n2=a[0][0].e2=1;
		for(i=0;i<h;++i)for(j=0;j<w;++j)
		{
			s=a[i][j];
			a[i][j+1].e1=(a[i][j+1].e1+s.e1+s.e2)%N;
			a[i][j+1].e2=s.n1;
			a[i+1][j].n1=(a[i+1][j].n1+s.n1+s.n2)%N;
			a[i+1][j].n2=s.e1;
		}
		s=a[h-1][w-1];
		printf("%d\n",(s.e1+s.e2+s.n1+s.n2)%N);
	}
	return 0;
}
```

After:

```c
#include <stdio.h>

int main(void){
	int i,j,k,w,h;
	while(scanf("%d%d",&w,&h),w,h){
		int dp[101][101][5]={0},ans=0;
		for(i=1;i<=h;i++) dp[i][1][4]=1;
		for(j=1;j<=w;j++) dp[1][j][4]=1;
		
		for(i=2;i<=h;i++){
			for(j=2;j<=w;j++){
				dp[i][j][0]=dp[i-1][j][3]+dp[i-1][j][4];
				dp[i][j][1]=dp[i][j-1][2]+dp[i][j-1][4];
				dp[i][j][2]=dp[i-1][j][0]+dp[i-1][j][2];
				dp[i][j][3]=dp[i][j-1][1]+dp[i][j-1][3];
				for(k=0;k<5;k++){
					if(dp[i][j][k]>=100000)dp[i][j][k]%=100000;
				}
			}
		}
		/*
		for(i=1;i<=h;i++){
			for(j=1;j<=w;j++){
				printf("(%f=%f=%f=%f=%f)%d%d\n",dp[i][j][0],dp[i][j][1],dp[i][j][2],dp[i][j][3],dp[i][j][4],i,j);
			}
		}
		*/
		for(i=0;i<5;i++){
			ans+=dp[h][w][i];
		}
		ans%=100000;
		printf("%d\n",ans);
	}
	return 0;
	
}
```

## Evidence

- Average coverage delta: 0.1578
- Source problems: train_AIZU_p00470
- Rule id: `dc9c69aa`
