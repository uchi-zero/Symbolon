---
name: avoid-recursive-modular-exponentiation-for-inverses
description: Use when the code computes modular inverses via Fermat's little theorem using a power function with a data-dependent loop (pw(x, mod-2)) called repeatedly inside combinatorial helpers
---

# avoid-recursive-modular-exponentiation-for-inverses

## Overview

A KLEE-coverage code transformation. Applying it rewrites C source so symbolic execution explores more of the program's behavior.

## When to use

When the code computes modular inverses via Fermat's little theorem using a power function with a data-dependent loop (pw(x, mod-2)) called repeatedly inside combinatorial helpers

## Transformation

Precompute modular inverses, factorials, and inverse factorials in a single linear pass using the recurrence inv[i] = (mod - mod/i) * inv[mod%i] % mod, then implement comb() as a straight-line multiplication of table lookups

## Why it helps

The pw() function executes ~30 iterations of branching on bits of (mod-2), and is invoked twice per combination call inside a symbolic loop, multiplying path counts exponentially; replacing it with table lookups eliminates these data-independent branches so KLEE spends its budget exploring input-dependent behavior instead of fixed arithmetic control flow

## Example

Before:

```c
#include<stdio.h>
int H,W,A,B,fac[222222];
#define mod 1000000007
int add(int x,int y){
	return (x+=y)<mod?x:x-mod;
}
int mul(int x,int y){
	return (int)((long long int)x*y%mod);
}
int pw(int x,int y){
	int r=1;
	while(1){
		if(y&1)r=mul(r,x);
		if(y>>=1)x=mul(x,x);else
			break;
	}
	return r;
}
int inv(int x){
	return pw(x,mod-2);
}
int c(int x,int y){
	--x;--y;
	return mul(fac[x+y],inv(mul(fac[x],fac[y])));
}
int main(){
	fac[0]=1;
	for(int i=1; i<222222; i++)
		fac[i] = mul(fac[i-1],i);
	scanf("%d%d%d%d",&H,&W,&A,&B);
	int res = 0;
	for(int i=B+1; i<=W; i++)
		res = add(res, mul(c(H-A,i),c(A, W-i+1)));
	printf("%d\n",res);
	return 0;
}
```

After:

```c
#include <stdio.h>

const long long mod = 1000000007;
long long inv[200200],fact[200200],ifact[200200];

long long comb(int n, int k)
{
	return fact[n] * ifact[k] % mod * ifact[n-k] % mod;
}

int main()
{
	inv[1] = fact[0] = fact[1] = ifact[0] = ifact[1] = 1;
	for (int i=2;i<200200;i++){
		inv[i] = (mod - mod / i) * inv[mod % i] % mod;
		fact[i] = fact[i-1] * i % mod;
		ifact[i] = ifact[i-1] * inv[i] % mod;
	}

	int h,w,a,b; scanf ("%d %d %d %d",&h,&w,&a,&b);

	long long ans = comb(h+w-2,h-1);

	for (int i=0;i<b;i++){
		long long u = comb(h-a-1+i,i);
		long long v = comb(a-1+w-1-i,a-1);
		ans = (ans - u * v % mod + mod) % mod;
	}
	printf ("%lld\n",ans);

	return 0;
}
```

## Evidence

- Average coverage delta: 0.3939
- Source problems: train_AtCoder_p04040
- Rule id: `a7ba2b7a`
