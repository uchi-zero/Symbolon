---
name: replace-lookup-table-with-explicit-structure
description: Use when adjacency or transition relations are encoded as opaque numeric lookup tables indexed by symbolic values, forcing KLEE to fork on every table access
---

# replace-lookup-table-with-explicit-structure

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When adjacency or transition relations are encoded as opaque numeric lookup tables indexed by symbolic values, forcing KLEE to fork on every table access

## Transformation

Replace the precomputed neighbor/transition table with explicit structural arithmetic (e.g., row/col coordinates with +1/-1 moves and bound checks) that directly expresses the relationship between states

## Why it helps

Symbolic indexing into data tables causes KLEE to fork on each possible index, multiplying paths; explicit arithmetic conditions yield simpler, more concrete branch constraints that the solver can resolve and merge efficiently, improving coverage of meaningful program states

## Example

Before:

```c
#include<stdio.h>
int main()
{
	int c,n,s,t,b,i,j,k,x[9][4]={
		{1,3,0,0},
		{0,2,4,1},
		{1,5,2,2},
		{0,4,6,3},
		{1,3,5,7},
		{2,4,8,5},
		{3,7,6,6},
		{4,6,8,7},
		{5,7,8,8}
	};
	char u,v,w;
	while(scanf("%d",&n),n)
	{
		int a[16][9]={0};
		scanf(" %c %c %c",&u,&v,&w);
		s=u-'A',t=v-'A',b=w-'A';
		c=a[0][s]=1;
		for(i=0;i<n;++i,c*=4)for(j=0;j<9;++j)for(k=0;k<4;++k)
			if(x[j][k]!=b)a[i+1][x[j][k]]+=a[i][j];
			else a[i+1][j]+=a[i][j];
		printf("%.8f\n",a[n][t]/(double)c);
	}
	return 0;
}
```

After:

```c
#include<stdio.h>
double dp[16][3][3];
char s[2];
char t[2];
char b[2];
int main(){
	int a;
	while(scanf("%d",&a),a){
		for(int i=0;i<16;i++)
			for(int j=0;j<3;j++)
				for(int k=0;k<3;k++)
					dp[i][j][k]=0;
		scanf("%s%s%s",s,t,b);
		int row=(s[0]-'A')/3;
		int col=(s[0]-'A')%3;
		int NGrow=(b[0]-'A')/3;
		int NGcol=(b[0]-'A')%3;
		dp[0][row][col]=1;
		for(int i=0;i<a;i++){
			for(int j=0;j<3;j++)
				for(int k=0;k<3;k++){
					if(j>0&&(j-1!=NGrow||k!=NGcol))dp[i+1][j-1][k]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
					if(k>0&&(j!=NGrow||k-1!=NGcol))dp[i+1][j][k-1]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
					if(j<2&&(j+1!=NGrow||k!=NGcol))dp[i+1][j+1][k]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
					if(k<2&&(j!=NGrow||k+1!=NGcol))dp[i+1][j][k+1]+=dp[i][j][k]*0.25;
					else dp[i+1][j][k]+=dp[i][j][k]*0.25;
				}
		}
		int Grow=(t[0]-'A')/3;
		int Gcol=(t[0]-'A')%3;
		printf("%.8lf\n",dp[a][Grow][Gcol]);
	}
}
```

## Evidence

- Average coverage delta: 0.4864
- Source problems: train_AIZU_p00606
- Rule id: `e22f0065`
