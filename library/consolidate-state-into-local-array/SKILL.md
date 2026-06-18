---
name: consolidate-state-into-local-array
description: Use when the code uses multiple separate global 2D arrays to represent different components of the same DP state, with updates spread across distinct named arrays
---

# consolidate-state-into-local-array

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code uses multiple separate global 2D arrays to represent different components of the same DP state, with updates spread across distinct named arrays

## Transformation

Merge the parallel arrays into a single locally-scoped multi-dimensional array indexed by an extra state dimension, initialized to zero each iteration, and rewrite the transitions as a uniform loop over that dimension

## Why it helps

A single zero-initialized local array gives KLEE a clean, bounded memory region per path with predictable aliasing, avoiding cross-test-case residual state in globals and reducing the number of distinct symbolic memory objects, which simplifies constraint solving and improves branch coverage during replay

## Example

Before:

```c
#include<stdio.h>
int dpRR[100][100];
int dpUR[100][100];
int dpUU[100][100];
int dpRU[100][100];
int main(){
	int a,b;
	while(scanf("%d%d",&a,&b),a){
		for(int i=0;i<a;i++)dpRR[i][0]=1;
		for(int i=0;i<b;i++)dpUU[0][i]=1;
		for(int i=1;i<a;i++){
			for(int j=1;j<b;j++){
				dpUR[i][j]=dpUU[i-1][j];
				dpRR[i][j]=(dpUR[i-1][j]+dpRR[i-1][j])%100000;
				dpRU[i][j]=dpRR[i][j-1];
				dpUU[i][j]=(dpRU[i][j-1]+dpUU[i][j-1])%100000;
				//printf("%d %d: %d %d %d %d\n",i,j,dpRR[i][j],dpUR[i][j],dpUU[i][j],dpRU[i][j]);
			}
		}
		int ret=(dpUR[a-1][b-1]+dpRR[a-1][b-1]+dpUU[a-1][b-1]+dpRU[a-1][b-1])%100000;
		printf("%d\n",ret);
	}
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

- Average coverage delta: 0.1277
- Source problems: train_AIZU_p00470
- Rule id: `2f16c58f`
