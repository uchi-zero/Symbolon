---
name: early-feasibility-pruning
description: Use when the code performs an expensive enumeration loop without first checking simple necessary conditions (e.g., divisibility by gcd) that would render the entire search infeasible
---

# early-feasibility-pruning

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code performs an expensive enumeration loop without first checking simple necessary conditions (e.g., divisibility by gcd) that would render the entire search infeasible

## Transformation

Add an early-exit feasibility check before the main loop (e.g., compute gcd of coefficients and verify the target is divisible by it, returning 0 immediately otherwise), and normalize variables by dividing out common factors

## Why it helps

Early termination on infeasible inputs lets KLEE prune large infeasible subtrees quickly, while normalization shrinks the numeric ranges of loop bounds and modular constraints, making path conditions easier for the constraint solver and increasing reachable coverage within the time budget

## Example

Before:

```c
#include<stdio.h>
typedef long long ll;
const int mod=998244353;
int mul(int a,int b){return a*(ll)b%mod;}
int ad(int a,int b){return(a+b)%mod;}
int pow(int a,int b){
	int s=1;
	while(b){
		if(b&1)s=mul(s,a);
		a=mul(a,a);
		b>>=1;
	}
	return s;
}
int fac[300010],rfac[300010];
int C(int n,int k){return mul(fac[n],mul(rfac[k],rfac[n-k]));}
int main(){
	int n,i,ans;
	ll a,b,k;
	scanf("%d%lld%lld%lld",&n,&a,&b,&k);
	fac[0]=1;
	for(i=1;i<=n;i++)fac[i]=mul(fac[i-1],i);
	rfac[n]=pow(fac[n],mod-2);
	for(i=n;i>0;i--)rfac[i-1]=mul(rfac[i],i);
	ans=0;
	for(i=0;i<=n&&i*a<=k;i++){
		if((k-i*a)%b==0&&(k-i*a)/b<=(ll)n)ans=ad(ans,mul(C(n,i),C(n,(k-i*a)/b)));
	}
	printf("%d",ans);
}
```

After:

```c
#include <stdio.h>
const int M=998244353;
long long f[300010]={1};
int gcd(int x,int y){return y?gcd(y,x%y):x;}
long long inv(long long x){
	long long t=1;
	for(int i=0;i<30;++i){
		if((M-2)&(1<<i))t=t*x%M;
		x=x*x%M;
	}
	return t;
}
long long c(int n,int r){
	return f[n]*inv(f[r])%M*inv(f[n-r])%M;
}
int main(){
	int i,n,x,y,t;
	long long a,b,k,ans=0;
	scanf("%d%lld%lld%lld",&n,&a,&b,&k);
	t=gcd(a,b);
	if(k%t)return !puts("0");
	for(i=1;i<=n;++i)f[i]=f[i-1]*i%M;
	a/=t,b/=t,k/=t;
	for(x=0;x<=n;++x){
		if(a*x>k)break;
		if((k-a*x)%b)continue;
		y=(k-a*x)/b;
		if(y<0||y>n)continue;
		ans=(ans+c(n,x)*c(n,y))%M;
	}
	printf("%lld",ans);
	return 0;
}
```

## Evidence

- Average coverage delta: 0.3282
- Source problems: train_AtCoder_p03332
- Rule id: `13bc5375`
