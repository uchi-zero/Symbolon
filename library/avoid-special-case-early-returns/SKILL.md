---
name: avoid-special-case-early-returns
description: Use when the code branches on equality of input parameters and returns early via specialized closed-form computations (e.g., Pow) for degenerate cases
---

# avoid-special-case-early-returns

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code branches on equality of input parameters and returns early via specialized closed-form computations (e.g., Pow) for degenerate cases

## Transformation

Remove special-case shortcut branches and compute the result through the unified general DP/iterative formulation that naturally handles all input combinations

## Why it helps

Early-return branches based on input equality create disjoint path partitions that KLEE must explore separately, and the closed-form Pow loop introduces input-dependent loop bounds that complicate symbolic constraints; a single unified computation yields fewer paths and more uniform constraints, letting generated tests cover more behaviors when replayed

## Example

Before:

```c
#include<stdio.h>
#pragma GCC optimize ("O2")
#define rep(i,N) for(int i=0;i<(int)N;i++)
const int MOD=998244353;
int Pow(int x,int y){int z=1;x%=MOD;while(y){if(y&1){z=(1ll*z*x)%MOD;}x=(1ll*x*x)%MOD;y>>=1;}return z;}
int main(void)
{
  int A,B,C,D,ans=0,sum,dp[3000]={0};
  if(!scanf("%d %d %d %d",&A,&B,&C,&D))return 0;
  if(A==C){return printf("%d\n",Pow(A,D-B)),0;}
  else if(B==D){return printf("%d\n",Pow(B,C-A)),0;}
  dp[A-1]=1;
  rep(i,D-B)
  {
    sum=0;
    rep(j,C)
    {
      sum=(1ll*sum*(B+i)%MOD+dp[j])%MOD;
      dp[j]=(1ll*j*dp[j]+sum)%MOD;
    }
  }
  rep(i,C){ans=(ans+1ll*dp[i]*Pow(D,C-i-1))%MOD;}
  return printf("%d\n",ans),0;
}
```

After:

```c
#include <stdio.h>
#define MOD 998244353
long long int DP[3010][3010];
int main()
{
	int a,b,c,d;
	scanf("%d%d%d%d",&a,&b,&c,&d);
	
	long long int C = 1;
	for(int j=c;j>=a;j--)
	{
		DP[d][j] = C;
		C*=d;
		C%=MOD;
	}
	
	for(int i=d-1;i>=b;i--)
	{
		C = 0;
		for(int j=c;j>=a;j--)
		{
			DP[i][j] = (C + j*DP[i+1][j])%MOD;
			
			//C *= i;
			C += DP[i+1][j];
			C *= i;
			C %= MOD;
			//printf("%d %d %lld!!\n",i,j,DP[i][j]);
		}
	}
	
	printf("%lld",DP[b][a]);
}
```

## Evidence

- Average coverage delta: 0.1754
- Source problems: train_AtCoder_p02634
- Rule id: `f438b999`
